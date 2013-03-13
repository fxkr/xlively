#include "main.h"

#include "model.h"
#include "view.h"
#include "ctrl.h"


int main(int argc, char *argv[]) {

    // Init modules in right order
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

