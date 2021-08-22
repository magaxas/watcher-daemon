#include "main.h"

int init_notify(config *conf)
{
    int fd = inotify_init();

    conf->fd = fd;
    conf->wd = (int *) malloc(conf->dirs_lenght * sizeof(int));
    if (conf->wd == NULL || fd == -1) {
        logger(ERROR, "Error occured while initializing inotify: fd=%d", fd);
        return 1;
    }

    for (int i = 0; i < conf->dirs_lenght; i++) {
        int wd = inotify_add_watch(fd, conf->dirs_to_watch[i], IN_CREATE);
        if (wd == -1) {
            logger(ERROR, "Failed to add watch with ID=%d", wd);
            continue;
        }
        else {
            logger(
                INFO,
                "Added \"%s\" to watch list of watcher with ID=%d",
                conf->dirs_to_watch[i], wd
            );
        }

        conf->wd[i] = wd;
    }

    return 0;
}

int get_event(config *conf, int (*callback)(config *conf, char *file_name, int wd))
{
    char buffer[BUF_LEN];
    int length, i = 0;
    
    length = read(conf->fd, buffer, BUF_LEN);
    if (length < 0) {
        logger(INFO, "Empty event.");
        return 0;
    }

    while (i < length) {
        struct inotify_event *event = (struct inotify_event *)&buffer[i];
        if (event->len) {
            if (event->mask & IN_CREATE && !(event->mask & IN_ISDIR)) {
                if (callback(conf, event->name, event->wd) == 1) return 1;
            }
            i += EVENT_SIZE + event->len;
        }
    }

    return 0;
}

void free_notify(config *conf)
{
    for (int i = 0; i < conf->dirs_lenght; i++) {
        if (inotify_rm_watch(conf->fd, conf->wd[i]) == -1) {
            logger(ERROR, "Could not close watcher with ID=%d", conf->wd[i]);
        }
    }

    close(conf->fd);
}
