#include "main.h"

void init_notify(config *conf)
{
    int fd = inotify_init();
    if (fd == -1)
    {
        logger(ERROR, "Failed to initialize inotify!");
        free_config(conf);
        free_notify(conf);
        exit(1);
    }

    conf->fd = fd;
    conf->wd = (int *)malloc(conf->dirs_lenght * sizeof(int));
    for (int i = 0; i < conf->dirs_lenght; i++)
    {
        int wd = inotify_add_watch(fd, conf->dirs_to_watch[i], IN_CREATE);
        if (wd == -1)
        {
            logger(ERROR, "Failed to add watch!");
            free_config(conf);
            free_notify(conf);
            exit(1);
        }
        else
        {
            logger(INFO, "Added dir to watch list...");
        }

        conf->wd[i] = wd; //log
    }
}

void get_event(config *conf, void (*callback)())
{
    char buffer[BUF_LEN];
    int length, i = 0;

    length = read(conf->fd, buffer, BUF_LEN);
    if (length < 0)
    {
        perror("read");
        exit(1);
    }

    while (i < length)
    {
        struct inotify_event *event = (struct inotify_event *)&buffer[i];
        if (event->len)
        {
            if (event->mask & IN_CREATE && !(event->mask & IN_ISDIR))
            {
                (*callback)(conf, &event->name, event->wd);
            }
            i += EVENT_SIZE + event->len;
        }
    }
}

void free_notify(config *conf)
{
    for (int i = 0; i < conf->dirs_lenght; i++)
    {
        if (inotify_rm_watch(conf->fd, conf->wd[i]) == -1)
        {
            logger(ERROR, "Could not close watcher!");
            close_log();
            exit(1);
        }
    }

    FREE(conf->wd);
    close(conf->fd);
}
