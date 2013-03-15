#ifndef CONFIG_H_
#define CONFIG_H_

#define CONFIG_MODE_WINDOW 1
#define CONFIG_MODE_FULLSCREEN 2
#define CONFIG_MODE_ROOT 3

typedef enum {
    WINDOW,
    ROOT,
    FULLSCREEN
} WindowMode;

typedef struct config_t {
    int width;
    int height;
    int pixel;
    int border;
    WindowMode mode;
} Config;
Config config;

void config_init(int argc, const char *argv[]);

#endif
