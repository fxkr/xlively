#ifndef MODEL_H_
#define MODEL_H_

#include<stdbool.h>

typedef struct model_t {
    int height;
    int width;
    char* array;
    char* next_array;
} Model;

void model_init(void);
void model_exit(void);
void model_configure(int width, int height);
void model_populate(void);
void model_populate_randomly(float percentage);
bool model_get(int x, int y);
void model_evolve(void);
int model_get_height(void);
int model_get_width(void);
void model_acquire_mutex(void);
void model_release_mutex(void);

#endif
