#include "view.h"

#include<X11/Xatom.h>
#include<X11/Xlib.h>
#include<X11/Xlibint.h>
#include<stdbool.h>
#include<stdlib.h>
#include<unistd.h>

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

void view_reinit_buffers(void);
bool view_handle(XEvent event);
Window view_get_root_window(Display *dpy);
Window view_get_new_window(Display *dpy);
Display *view_get_display(void);
void view_redraw(void);


void view_init() {
    XInitThreads(); // we are multithreaded; must be first Xlib call

    dpy = view_get_display();
    wnd = view_get_new_window(dpy);

    Colormap map = DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy));
    XColor exactColor; // don't care
    XAllocNamedColor(dpy, map, "black", &black, &exactColor);
    XAllocNamedColor(dpy, map, "white", &white, &exactColor);

    XGetWindowAttributes(dpy, wnd, &wnd_attr);
    XSelectInput(dpy, wnd, ExposureMask);
    view_reinit_buffers();

    Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", false);

    XSetWMProtocols(dpy, wnd, &wmDeleteMessage, 1);
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
    Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", false);
    if (event.type == Expose) {
        while (XCheckWindowEvent(dpy, wnd, ExposureMask, &event)) {
        }
        int old_w = wnd_attr.width, old_h = wnd_attr.height;
        XGetWindowAttributes(dpy, wnd, &wnd_attr);
        if (old_w != wnd_attr.width || old_h != wnd_attr.height) {
            view_reinit_buffers();
        }
        view_redraw();
    } else if (event.type == ClientMessage && event.xclient.data.l[0] == wmDeleteMessage) {
        return false; // stop event loop
    }
    return true;
}

void view_exit() {
    XDestroyWindow(dpy, wnd);
    XCloseDisplay(dpy);
}

void view_notify() {
    XClientMessageEvent xevent;
    xevent.type = Expose;
    XSendEvent(dpy, wnd, 0, 0, (XEvent *)&xevent);
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
    XSetBackground(dpy, gc, white.pixel);
    XSetForeground(dpy, gc, white.pixel);
    XSetBackground(dpy, gc_clip, 1);
    XSetForeground(dpy, gc_clip, 1);
    for (int y = 0; y < model_get_height(); y++) {
        for (int x = 0; x < model_get_width(); x++) {
            if (model_get(x, y)) {
                XFillRectangle(dpy, wnd_buffer, gc, x*25, y*25, 20, 20);
                XFillRectangle(dpy, clip_mask, gc_clip, x*25, y*25, 20, 20);
            }
        }
    }

    // Copy to window
    XSetClipMask(dpy, gc, clip_mask);
    XCopyArea(dpy, wnd_buffer, wnd, gc, 0, 0, wnd_attr.width, wnd_attr.height, 0, 0);

    model_release_mutex();
}
