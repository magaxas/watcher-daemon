#include "main.h"

void move_file(config *conf, char *file_name, int wd)
{
    for (int i = 0; i < conf->watchers_length; i++)
    {
        for (int j = 0; j < conf->watchers[i].types_length; j++)
        {
            char *ext = get_ext(file_name);
            if (strcmp(ext, conf->watchers[i].file_types[j]) == 0)
            {
                char *old_path = (char *)calloc(
                    sizeof(char), (strlen(file_name) + strlen(conf->dirs_to_watch[wd - 1]) + 1) * sizeof(char));
                strcat(old_path, conf->dirs_to_watch[wd - 1]);
                strcat(old_path, file_name);

                char *new_path = (char *)calloc(
                    sizeof(char), (strlen(file_name) + strlen(conf->watchers[i].dir_to_move) + 1) * sizeof(char));
                strcat(new_path, conf->watchers[i].dir_to_move);
                strcat(new_path, file_name);

                //Move file to specified directory
                if (rename(old_path, new_path) == -1)
                {
                    logger(WARNING, "Error occured while moving file: %s", strerror(errno));
                }
                else
                {
                    logger(
                        INFO,
                        "Succesfully moved file \"%s\" from directory \"%s\" to \"%s\"",
                        file_name, conf->dirs_to_watch[wd - 1], conf->watchers[i].dir_to_move);
                }

                FREE(old_path);
                FREE(new_path);
            }
            FREE(ext);
        }
    }
}

int main(int argc, char **argv)
{
    open_log("/home/magax/Desktop/watcher-daemon/", "logger.log");
    logger(INFO, "Started program...");

    config *conf = init_config("../config.json");
    init_notify(conf);

    while (1)
    {
        get_event(conf, &move_file);
        sleep(1);
    }

    free_notify(conf);
    free_config(conf);
    close_log();
    return 0;
}
