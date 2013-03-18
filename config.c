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
    config.grid = 32;
    config.border = 4;
    config.mode = FULLSCREEN;

    // Declare options
    struct poptOption optionsTable[] = {
        { "width", 'w', POPT_ARG_INT, &config.width, 0,
            "use a fixed number of cells horizontally" },
        { "height", 'h', POPT_ARG_INT, &config.height, 0,
            "use a fixed number of cells vertically" },
        { "grid", 'g', POPT_ARG_INT, &config.grid, 0,
            "grid width, in pixel" },
        { "border", 'b', POPT_ARG_INT, &config.border, 0,
            "border around cells, in pixel" },
        { "window", NULL, POPT_ARG_VAL, &config.mode, 0,
            "run in windowed mode" },
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
    } else if (config.grid < 1 || config.grid > 256) {
        usage(context, "grid", "not in [1..256]");
    } else if (!(config.grid > 2 * config.border)) {
        usage(context, "grid", "must be at least twice as large as border");
    }

    poptFreeContext(context);
}

