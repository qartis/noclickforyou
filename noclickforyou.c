#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>
#include <X11/Xlib.h>

Display *display;
Window window;

void restore(int signum)
{
	(void)signum;

	XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, None);
    XFlush(display);
    XCloseDisplay(display);

	exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <window_id>\n", argv[0]);
        return 1;
    }

    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open display\n");
        return 1;
    }

    window = strtoul(argv[1], NULL, 0);

    int event_base, error_base;
    if (!XFixesQueryExtension(display, &event_base, &error_base)) {
        fprintf(stderr, "XFixes extension not available\n");
        XCloseDisplay(display);
        return 1;
    }

    XserverRegion empty_region = XFixesCreateRegion(display, NULL, 0);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, empty_region);
    XFixesDestroyRegion(display, empty_region);
    XFlush(display);

    signal(SIGINT, restore);
    signal(SIGTERM, restore);

	for (;;) {
		pause();
	}
}
