#include "model.h"

#include<pthread.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>


Model model;
pthread_mutex_t model_mutex;


int xy(int x, int y);
void model_set(int x, int y, _Bool alive);
int model_get_neighbors(int x, int y);
void model_flip(void);


void model_init() {
    srand(time(NULL));
    model.width = 0;
    model.height = 0;
    free(model.array);
    free(model.next_array);
}

void model_exit() {
    model_init();
}

void model_configure(int width, int height) {
    model_init();
    model.array = malloc((sizeof *(model.array)) * width * height);
    model.next_array = malloc((sizeof *(model.next_array)) * width * height);
    model.width = width;
    model.height = height;
    for (int i = 0; i < width * height; i++) {
        model.array[i] = 0;
    }
}

/* Set she cells at the border to "dead" (don't modify the other cells). */
void model_populate_border() {
    for (int x = 0; x < model.width; x++) {
        model.array[xy(x, 0)] = 0;
        model.array[xy(x, model.height - 1)] = 0;
        model.next_array[xy(x, 0)] = 0;
        model.next_array[xy(x, model.height - 1)] = 0;
    }
    for (int y = 1; y < model.height - 1; y++) {
        model.array[xy(0, y)] = 0;
        model.array[xy(model.width - 1, y)] = 0;
        model.next_array[xy(0, y)] = 0;
        model.next_array[xy(model.width - 1, y)] = 0;
    }
}

/* Set some percentage of cells to "alive" and the remaining ones to "dead".
 * Cells at the border will always be dead. */
void model_populate_randomly(float percentage) {
    model_populate_border();
    for (int y = 1; y < model.height - 1; y++) {
        for (int x = 1; x < model.width - 1; x++) {
            model_set(x, y, ((double)rand() / RAND_MAX) < percentage);
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

int xy(int x, int y) {
    return x + y * model.width;
}

void model_set(int x, int y, bool alive) {
    model.array[xy(x, y)] = alive;
}

bool model_get(int x, int y) {
    return model.array[xy(x, y)];
}

void model_evolve() {
    static int lookup_table[16] = {
        // number of neighbors
        0, 0, 0, 1, 0, 0, 0, 0, // cell was dead
        0, 0, 1, 1, 0, 0, 0, 0  // cell was alive
    };

    char* array = model.array;
    char* next_array = model.next_array;
    int height = model.height;
    int width = model.width;
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int mid = y * width + x;
            int top = mid - width;
            int bot = mid + width;
            int num_alive =
                array[top-1] + array[top] + array[top+1] +
                array[mid-1] +              array[mid+1] +
                array[bot-1] + array[bot] + array[bot+1];
            next_array[mid] = lookup_table[num_alive | (array[mid] << 3)];
        }
    }

    // Switch front/back buffer
    model_acquire_mutex();
    char *temp = model.array;
    model.array = model.next_array;
    model.next_array = temp;
    model_release_mutex();
}

