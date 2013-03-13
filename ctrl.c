#include "ctrl.h"

#include<errno.h>
#include<pthread.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

#include "model.h"
#include "view.h"


void ctrl_thread_start(void);
void ctrl_thread_run(void *ptr);


void ctrl_init() {
}

void ctrl_run() {
    model_configure(20, 20);
    model_populate_randomly(0.2);
    ctrl_thread_start();
    view_run();
}

void ctrl_thread_start() {
    pthread_t thread;
    pthread_create(&thread, NULL, (void *) &ctrl_thread_run, NULL);
}

void ctrl_thread_run(void *ptr) {
    while (true) {
        sleep(1);
        model_acquire_mutex();
        model_evolve();
        model_release_mutex();
        view_notify();
    }
}

void ctrl_exit() {
}

