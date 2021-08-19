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
                    (strlen(file_name) + strlen(conf->dirs_to_watch[wd - 1]) + 1) *
                        sizeof(char),
                    1);
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
                    (strlen(file_name) + strlen(conf->watchers[i].dir_to_move) + 1) *
                        sizeof(char),
                    1);
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

                // Move file to specified directory
                if (rename(old_path, new_path) == -1)
                {
                    logger(WARNING, "Error occured while moving file: %s",
                           strerror(errno));
                }
                else
                {
                    logger(INFO,
                           "Watcher \"%s\" in action. Succesfully moved file \"%s\" from "
                           "directory \"%s\" to \"%s\"",
                           conf->watchers[i].name, file_name, conf->dirs_to_watch[wd - 1],
                           conf->watchers[i].dir_to_move);
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

void print_help(void)
{
    printf("Options for watcher-daemon:\n");
    printf("  -h --help                  Print help menu\n");
    printf("  -c --conf_file filepath    Read config file\n");
    printf("  -l --log_file filepath     Write logs to this file\n");
    printf("  -p --pid_file filepath     Write child PID (optional)\n");
    printf("  -d --default_home dirpath  Your home directory (optional)\n");
}

int main(int argc, char **argv)
{
    static struct option long_options[] = {
        {"conf_file", required_argument, 0, 'c'},
        {"log_file", required_argument, 0, 'l'},
        {"pid_file", required_argument, 0, 'p'},
        {"default_home", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {NULL, 0, 0, 0}};

    char *log_file = NULL, *config_path = NULL, *pid_file = NULL, *default_home = NULL;
    int val, option_index = 0;

    while ((val = getopt_long(argc, argv, "c:l:p:d:h", long_options, &option_index)) != -1)
    {
        switch (val)
        {
        case 'c':
            config_path = strdup(optarg);
            break;
        case 'l':
            log_file = strdup(optarg);
            break;
        case 'p':
            pid_file = strdup(optarg);
            break;
        case 'd':
            default_home = strdup(optarg);
            break;
        case 'h':
            print_help();
            return EXIT_SUCCESS;
        case '?':
            print_help();
            return EXIT_FAILURE;
        default:
            break;
        }
    }

    if (log_file == NULL || config_path == NULL)
    {
        perror("Missing mandatory parameters");
        exit(1);
    }

    //Start if not normal mode
    daemonize(pid_file);

    //Start logger
    open_log(log_file);
    logger(INFO, "Started program...");
    logger(INFO, "Current process ID=%d", (int)getpid());

    //Initialize config and inotify
    config *conf = init_config(config_path, default_home);
    init_notify(conf);

    //Handle signals
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = &handle_signal;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGINT, &act, NULL) < 0 || sigaction(SIGTERM, &act, NULL) < 0)
    {
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
    MFREE(4, log_file, config_path, pid_file, default_home);

    return 0;
}
