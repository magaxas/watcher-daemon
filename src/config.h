#pragma once

typedef struct watcher
{
    int types_length;
    char **file_types;
    char *name;
    char *dir_to_move;
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

config *init_config(char *conf_name, char *default_home);
void free_config(config *conf);
