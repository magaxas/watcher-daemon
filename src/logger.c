#include "main.h"

static FILE *log = NULL;

int open_log(char *file)
{
    int rc = 0;
    log = fopen(file, "a");
    if (log == NULL)
    {
        rc = 1;
    }
    return rc;
}

int close_log()
{
    int rc = 0;
    if (log != NULL)
    {
        rc = fclose(log);
    }
    return rc;
}

static int get_level(int level, char *msg)
{
    int rc = 0;
    switch (level)
    {
    case INFO:
        strcpy(msg, "[INFO]");
        break;

    case WARNING:
        strcpy(msg, "[WARNING]");
        break;

    case ERROR:
        strcpy(msg, "[ERROR]");
        break;

    default:
        rc = 1;
        break;
    }
    return rc;
}

void logger(int level, char *msg)
{
    int rc;
    char msg_lvl[15];
    if (log == NULL)
    {
        return;
    }

    rc = get_level(level, msg_lvl);
    if (rc > 0)
    {
        return;
    }
    fprintf(log, "%s %s %s %s\n", msg_lvl, __DATE__, __TIME__, msg);
    fflush(log);
}
