xlively
=======

This is a simple implementation of [Conway's Game of Life][1] that
can be used as an X11 screensaver and/or animated desktop background.
I wrote it for fun, to practice C and to get to know Xlib.

* Author: Felix Kaiser (felix.kaiser@fxkr.net)
* License: revised BSD (see LICENSE)
* Dependencies: Xlib, popt
* Runs on: Linux


How to use
----------

Build it with `make`. Then you can:

* just run it in fullscreen mode: `xlively`
* use it as an animated desktop background, run `xlively --root`.
* use it with xscreensaver: add `xlively --xs \n\` to the `programs`
section in `~/.xscreensaver`, then select it in `xscreensaver-demo`.

There are various command line options; see `xlively --help`.


Notes
-----

I should probably have built this on top of xscreensaver (which
would have provided some useful utilities and infrastructure),
but I really wanted to write everything myself.


[1]: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

