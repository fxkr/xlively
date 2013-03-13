#ifndef MODEL_H_
#define MODEL_H_

#include<stdbool.h>

typedef struct model_t {
    int height;
    int width;
    bool* array;
} Model;

void model_init(void);
void model_exit(void);
void model_configure(int width, int height);
void model_populate(void);
bool model_is_valid_coord(int x, int y);
int model_coord_to_index(int x, int y);
void model_set(int x, int y, _Bool alive);
bool model_get(int x, int y);
bool model_get_int(int x, int y);
int model_get_neighbors(int x, int y);
void model_evolve(void);
int model_get_height(void);
int model_get_width(void);
void model_acquire_mutex(void);
void model_release_mutex(void);

#endif
