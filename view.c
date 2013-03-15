#include "view.h"

#include<X11/Xatom.h>
#include<X11/Xlib.h>
#include<X11/Xlibint.h>
#include<stdbool.h>
#include<stdlib.h>
#include<unistd.h>

#include "config.h"
#include "model.h"


XColor black;
XColor white;

Display *dpy;
Window wnd;
XWindowAttributes wnd_attr;
Pixmap wnd_buffer;
Pixmap clip_mask;
GC gc;
GC gc_clip;

Atom wmDeleteMessage;
Atom wmPleaseRedrawMessage;


void view_set_fullscreen(bool state);
void view_reinit_buffers(void);
bool view_handle(XEvent event);
Window view_get_root_window(Display *dpy);
Window view_get_new_window(Display *dpy);
Display *view_get_display(void);
void view_redraw(void);


void view_init() {
    XInitThreads(); // we are multithreaded; must be first Xlib call

    dpy = view_get_display();
    if (config.mode == WINDOW) {
        wnd = view_get_new_window(dpy);
    } else if (config.mode == FULLSCREEN) {
        wnd = view_get_new_window(dpy);
        view_set_fullscreen(true);
    } else if (config.mode == ROOT) {
        wnd = view_get_root_window(dpy);
    }

    Colormap map = DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy));
    XColor exactColor; // don't care
    XAllocNamedColor(dpy, map, "black", &black, &exactColor);
    XAllocNamedColor(dpy, map, "white", &white, &exactColor);

    XGetWindowAttributes(dpy, wnd, &wnd_attr);
    XSelectInput(dpy, wnd, ExposureMask | StructureNotifyMask);
    view_reinit_buffers();

    wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", false);
    wmPleaseRedrawMessage = XInternAtom(dpy, "WM_PLEASE_REDRAW", false); // custom

    XSetWMProtocols(dpy, wnd, &wmDeleteMessage, 1);
}

void view_set_fullscreen(bool state) {
    if (wnd == None && wnd != DefaultRootWindow(dpy)) {
        return;
    }

    Atom atomFullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", True);
    Atom atomState = XInternAtom(dpy, "_NET_WM_STATE", True);
    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.window = wnd;
    event.xclient.message_type = atomState;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 1;
    event.xclient.data.l[1] = atomFullscreen;
    event.xclient.data.l[2] = 0;
    XSendEvent(dpy, DefaultRootWindow(dpy), 0,
        SubstructureRedirectMask | SubstructureNotifyMask,
        &event);
    XFlush(dpy);
}

void view_reinit_buffers() {
    gc = XCreateGC(dpy, wnd, 0, NULL);
    wnd_buffer = XCreatePixmap(dpy, wnd, wnd_attr.width, wnd_attr.height, wnd_attr.depth);
    clip_mask = XCreatePixmap(dpy, wnd_buffer, wnd_attr.width, wnd_attr.height, 1);
    gc_clip = XCreateGC(dpy, clip_mask, 0, NULL);
}

void view_run() {
    view_redraw();
    XEvent event;
    do {
        XNextEvent(dpy, &event);
    } while (view_handle(event));
}

bool view_handle(XEvent event) {
    if (event.type == ConfigureNotify) {
        int old_w = wnd_attr.width, old_h = wnd_attr.height;
        XGetWindowAttributes(dpy, wnd, &wnd_attr);
        if (old_w != wnd_attr.width || old_h != wnd_attr.height) {
            view_reinit_buffers();
        }
    } else if (event.type == Expose &&
               event.xexpose.count == 0) {
        view_redraw();
    } else if (event.type == ClientMessage &&
               event.xclient.message_type == wmPleaseRedrawMessage) {
        view_redraw();
    } else if (event.type == ClientMessage &&
               event.xclient.data.l[0] == wmDeleteMessage) {
        return false; // stop event loop
    }
    return true;
}

void view_exit() {
    XDestroyWindow(dpy, wnd);
    XCloseDisplay(dpy);
}

void view_notify() {
    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.display = dpy;
    event.xclient.window = wnd;
    event.xclient.message_type = wmPleaseRedrawMessage;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 0;
    event.xclient.data.l[1] = 0;
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;
    XSendEvent(dpy, wnd, 0, ExposureMask, &event);
    XFlush(dpy);
}

Window view_get_root_window(Display *dpy) {
    return RootWindow(dpy, DefaultScreen(dpy));
}

Window view_get_new_window(Display *dpy) {
    Window w = XCreateSimpleWindow(
        dpy, view_get_root_window(dpy),
        0, 0, 640, 480, 0,
        WhitePixel(dpy, DefaultScreen(dpy)),
        BlackPixel(dpy, DefaultScreen(dpy)));
    XStoreName(dpy, w, "Game of life");
    XSelectInput(dpy, w, StructureNotifyMask);
    XMapRaised(dpy, w);
    XEvent e;
    do {
        XWindowEvent(dpy, w, StructureNotifyMask, &e);
    } while (e.type != MapNotify);
    return w;
}

Display* view_get_display() {
    return XOpenDisplay(getenv("DISPLAY"));
}

void view_redraw() {
    model_acquire_mutex();

    // Background and clipmask
    XSetBackground(dpy, gc, black.pixel);
    XSetForeground(dpy, gc, black.pixel);
    XFillRectangle(dpy, wnd_buffer, gc, 0, 0, wnd_attr.width, wnd_attr.height);
    XSetBackground(dpy, gc_clip, 0);
    XSetForeground(dpy, gc_clip, 0);
    XFillRectangle(dpy, clip_mask, gc_clip, 0, 0, wnd_attr.width, wnd_attr.height);

    // Draw cells
    XSetBackground(dpy, gc_clip, 1);
    XSetForeground(dpy, gc_clip, 1);
    for (int y = 0; y < model_get_height(); y++) {
        for (int x = 0; x < model_get_width(); x++) {
            if (model_get(x, y)) {
                XSetBackground(dpy, gc, white.pixel);
                XSetForeground(dpy, gc, white.pixel);
            } else {
                XSetBackground(dpy, gc, black.pixel);
                XSetForeground(dpy, gc, black.pixel);
            }
            XFillRectangle(dpy, wnd_buffer, gc, x*25, y*25, 20, 20);
            XFillRectangle(dpy, clip_mask, gc_clip, x*25, y*25, 20, 20);
        }
    }

    // Copy to window
    XSetClipMask(dpy, gc, clip_mask);
    XCopyArea(dpy, wnd_buffer, wnd, gc, 0, 0, wnd_attr.width, wnd_attr.height, 0, 0);

    model_release_mutex();
}

