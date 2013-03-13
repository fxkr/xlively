#ifndef VIEW_H_
#define VIEW_H_

#include<X11/Xlib.h>
#include<stdbool.h>

void view_init(void);
void view_run(void);
void view_exit(void);
void view_notify(void);

#endif
