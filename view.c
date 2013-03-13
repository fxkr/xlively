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
GC gc;


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
    gc = XCreateGC(dpy, wnd, 0, NULL);
    XSelectInput(dpy, wnd, ExposureMask);
    wnd_buffer = XCreatePixmap(dpy, wnd, wnd_attr.width, wnd_attr.height, wnd_attr.depth);

    Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", false);

    XSetWMProtocols(dpy, wnd, &wmDeleteMessage, 1);
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
        XGetWindowAttributes(dpy, wnd, &wnd_attr);
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

    // Background
    XSetBackground(dpy, gc, black.pixel);
    XSetForeground(dpy, gc, black.pixel);
    XFillRectangle(dpy, wnd_buffer, gc, 0, 0, wnd_attr.width, wnd_attr.height);

    // Draw cells
    XSetBackground(dpy, gc, white.pixel);
    XSetForeground(dpy, gc, white.pixel);
    for (int y = 0; y < model_get_height(); y++) {
        for (int x = 0; x < model_get_width(); x++) {
            if (model_get(x, y)) {
                XFillRectangle(dpy, wnd_buffer, gc, x*25, y*25, 20, 20);
            }
        }
    }

    // Copy to window
    XCopyArea(dpy, wnd_buffer, wnd, gc, 0, 0, wnd_attr.width, wnd_attr.height, 0, 0);

    model_release_mutex();
}

