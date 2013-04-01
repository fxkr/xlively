#include "main.h"

#include "config.h"
#include "model.h"
#include "view.h"
#include "ctrl.h"


int main(int argc, const char *argv[]) {
    config_init(argc, argv);
    model_init();

    model_configure(2048, 1024);
    model_populate_randomly(0.2);

    for (int i = 0; i < 1000; i++) {
        model_evolve();
    }

    model_exit();
    return 0;
}

