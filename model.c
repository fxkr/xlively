#include "model.h"

#include<pthread.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>


Model model;
pthread_mutex_t model_mutex;

void model_init() {
    srand(time(NULL));
    model.width = 0;
    model.height = 0;
    free(model.array);
}

void model_exit() {
    model_init();
}

void model_configure(int width, int height) {
    model_init();
    model.array = malloc((sizeof *(model.array)) * width * height);
    model.width = width;
    model.height = height;
    for (int y = 0; y < model.height; y++) {
        for (int x = 0; x < model.width; x++) {
            model_set(x, y, false);
        }
    }
}

void model_populate() {
    if (model_is_valid_coord(1, 2)) {
        model_set(1, 2, true);
    }
    if (model_is_valid_coord(1, 2)) {
        model_set(2, 2, true);
    }
    if (model_is_valid_coord(1, 2)) {
        model_set(3, 2, true);
    }
}

void model_populate_randomly(float percentage) {
    for (int y = 0; y < model.height; y++) {
        for (int x = 0; x < model.width; x++) {
            model_set(x, y, ((double)rand() / RAND_MAX) < percentage);
        }
    }
}

bool model_is_valid_coord(int x, int y) {
    return (0<x) && (x<model.width) && (0<y) && (y<model.height);
}

int model_coord_to_index(int x, int y) {
    return x + y * model.width;
}

void model_set(int x, int y, bool alive) {
    if (model_is_valid_coord(x, y)) {
        model.array[model_coord_to_index(x, y)] = alive;
    }
}

bool model_get(int x, int y) {
    return model_is_valid_coord(x, y) ? model.array[model_coord_to_index(x, y)] : false;
}

bool model_get_int(int x, int y) {
    return model_get(x, y) ? 1 : 0;
}

int model_get_neighbors(int x, int y) {
    return model_get_int(x - 1, y - 1)
        +  model_get_int(x    , y - 1)
        +  model_get_int(x + 1, y - 1)
        +  model_get_int(x - 1, y    )
        +  model_get_int(x + 1, y    )
        +  model_get_int(x - 1, y + 1)
        +  model_get_int(x    , y + 1)
        +  model_get_int(x + 1, y + 1);
}

void model_evolve() {

    // Count cell neighbors
    int counts[model.width * model.height];
    for (int y = 0; y < model.height; y++) {
        for (int x = 0; x < model.width; x++) {
            counts[model_coord_to_index(x, y)] = model_get_neighbors(x, y);
        }
    }

    // Update cell states
    for (int y = 0; y < model.height; y++) {
        for (int x = 0; x < model.width; x++) {
            bool alive = counts[model_coord_to_index(x, y)] == 3 ||
                (counts[model_coord_to_index(x, y)] == 2 && model_get(x, y));
            model_set(x, y, alive);
        }
    }
}

int model_get_height() {
    return model.height;
}

int model_get_width() {
    return model.width;
}

void model_acquire_mutex() {
    pthread_mutex_lock(&model_mutex);
}

void model_release_mutex() {
    pthread_mutex_unlock(&model_mutex);
}

