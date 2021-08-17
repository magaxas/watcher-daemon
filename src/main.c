#include "main.h"

void move_file(config *conf, char *file_name, int wd)
{
    for (int i = 0; i < conf->watchers_length; i++)
    {
        for (int j = 0; j < conf->watchers[i].types_length; j++)
        {
            char *ext = (char*) calloc(sizeof(char), (strlen(conf->watchers[i].file_types[j])+3) * sizeof(char));
            ext[0] = '.';
            strcat(ext, conf->watchers[i].file_types[j]);

            if (strcmp(getExt(file_name), ext) == 0)
            {
                char *dtw = (char*) calloc(sizeof(char), (strlen(file_name)+strlen(conf->dirs_to_watch[wd])+1) * sizeof(char));
                char *old_path = strcat(dtw, file_name);
                char *new_path = conf->watchers[i].dir_to_move;

                //Move file to specified directory
                rename(old_path, new_path);
                printf("matched\n");
                FREE(dtw);
            }
            FREE(ext);
        }
    }
}

int main(int argc, char **argv)
{
    config *conf = init_config("../config.json");
    init_notify(conf);

    while (1)
    {
        get_event(conf, &move_file);
    }

    free_notify(conf);
    free_config(conf);
    return 0;
}
