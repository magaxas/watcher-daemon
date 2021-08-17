#pragma once

typedef struct watcher
{
    int types_length;
    char **file_types;
    char *name;
    char *dir_to_move;
    bool enabled;
} watcher;

typedef struct config
{
    int dirs_lenght;
    char **dirs_to_watch;
    int watchers_length;
    watcher *watchers;

    //Inotify
    int fd, *wd;
} config;

extern config *init_config();
extern void free_config(config *conf);
extern void print_config(config *conf);
