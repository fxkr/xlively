#include "config.h"

#include <popt.h>
#include <stdio.h>
#include <stdlib.h>


Config config;

void usage(poptContext context, const char *error_type, const char *error_msg) {
    poptPrintUsage(context, stderr, 0);
    if (error_type && error_msg) {
        fprintf(stderr, "error: %s: %s\n", error_type, error_msg);
    } else if (error_type) {
        fprintf(stderr, "error: %s\n", error_type);
    }
    exit(1);
}

void config_init(int argc, const char *argv[]) {

    // Set default values
    config.width = 0;
    config.height = 0;
    config.pixel = 32;
    config.border = 8;
    config.mode = WINDOW;

    // Declare options
    struct poptOption optionsTable[] = {
        { "width", 'w', POPT_ARG_INT, &config.width, 0,
            "use a fixed number of cells horizontally" },
        { "height", 'h', POPT_ARG_INT, &config.height, 0,
            "use a fixed number of cells vertically" },
        { "pixel", 'p', POPT_ARG_INT, &config.pixel, 0,
            "edge length of cells, in pixel" },
        { "border", 'p', POPT_ARG_INT, &config.border, 8,
            "space between cells, in pixel" },
        { "root", 'r', POPT_ARG_VAL, &config.mode, ROOT,
            "use root window" },
        { "fullscreen", 'f', POPT_ARG_VAL, &config.mode, FULLSCREEN,
            "use a fullscreen window" },
        POPT_AUTOHELP
        { NULL, 0, 0, NULL, 0 }
    };

    // Parsing
    poptContext context = poptGetContext(NULL, argc, argv, optionsTable, 0);
    int rc = poptGetNextOpt(context);
    if (rc != -1) {
        usage(context,
            poptBadOption(context, POPT_BADOPTION_NOALIAS),
            poptStrerror(rc));
    }

    // Check values for sanity (somewhat arbitrary)
    if (poptPeekArg(context) != NULL) {
        usage(context, "extra argument(s)", NULL);
    } else if (config.width < 0 || config.width > 4096) {
        usage(context, "width", "not in [0..4096]");
    } else if (config.height < 0 || config.height > 4096) {
        usage(context, "height", "not in [0..4096]");
    } else if (config.border < 1 || config.border > 256) {
        usage(context, "border", "not in [1..256]");
    } else if (config.pixel < 1 || config.pixel > 256) {
        usage(context, "pixel", "not in [1..256]");
    }

    poptFreeContext(context);
}
