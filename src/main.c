#include "main.h"

static int running = 1;

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
                    (strlen(file_name) + strlen(conf->dirs_to_watch[wd - 1]) + 1) * sizeof(char), 1);
                if (old_path == NULL)
                {
                    logger(ERROR, "Error occured while allocating \"old_path\"");
                    free_notify(conf);
                    free_config(conf);
                    close_log();
                    exit(1);
                }
                strcat(old_path, conf->dirs_to_watch[wd - 1]);
                strcat(old_path, file_name);

                char *new_path = (char *)calloc(
                    (strlen(file_name) + strlen(conf->watchers[i].dir_to_move) + 1) * sizeof(char), 1);
                if (new_path == NULL)
                {
                    logger(ERROR, "Error occured while allocating \"new_path\"");
                    free_notify(conf);
                    free_config(conf);
                    close_log();
                    exit(1);
                }
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

void handle_signal(int sig, siginfo_t *siginfo, void *context)
{
    logger(INFO, "Stopping daemon...");
    running = 0;
}

int main(int argc, char **argv)
{
    //Start daemon
    daemonize();

    //Start logger
    open_log("/home/magax/Desktop/watcher-daemon/", "logger.log");
    logger(INFO, "Started program...");
    logger(INFO, "Current process ID=%d", (int)getpid());

    //Initialize config and inotify
    config *conf = init_config("/home/magax/projects/watcher-daemon/config4.json");
    init_notify(conf);

    //Handle signals
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
 	act.sa_sigaction = &handle_signal;
 	act.sa_flags = SA_SIGINFO;
    
	if (sigaction(SIGINT, &act, NULL) < 0 || sigaction(SIGTERM, &act, NULL) < 0) {
		logger(ERROR, "Failed to register sigaction!");
        goto end;
	}

    //Main loop
    while (running)
    {
        get_event(conf, &move_file);
    }

end:
    free_notify(conf);
    free_config(conf);
    
    logger(INFO, "Exiting program...");
    close_log();

    return 0;
}
