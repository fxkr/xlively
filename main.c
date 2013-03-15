#include "main.h"

#include "config.h"
#include "model.h"
#include "view.h"
#include "ctrl.h"


int main(int argc, const char *argv[]) {

    // Init modules in right order
    config_init(argc, argv);
    model_init();
    view_init();
    ctrl_init();

    // Run controller
    ctrl_run();

    // Exit in reverse order
    ctrl_exit();
    view_exit();
    model_exit();
    return 0;
}

