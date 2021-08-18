#include "main.h"

static FILE *log = NULL;

int open_log(char *dir, char *file)
{
    int rc = 0;
    char *path = (char *)calloc(1, strlen(dir) + strlen(file) + 1);
    recursive_mkdir(dir);
    strcat(path, dir);
    strcat(path, file);

    log = fopen(path, "a");
    if (log == NULL)
    {
        rc = 1;
    }
    FREE(path);
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

static int vasprintf(char **strp, const char *fmt, va_list ap)
{
    va_list ap2;
    va_copy(ap2, ap);
    char tmp[1];
    int size = vsnprintf(tmp, 1, fmt, ap2);
    if (size <= 0)
        return size;
    va_end(ap2);
    size += 1;
    *strp = (char *)malloc(size * sizeof(char));
    return vsnprintf(*strp, size, fmt, ap);
}

void logger(int level, char *fmt, ...)
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

    va_list l;
    char *msg = NULL;
    va_start(l, fmt);
    vasprintf(&msg, fmt, l);
    va_end(l);

    fprintf(log, "%s %s %s %s\n", msg_lvl, __DATE__, __TIME__, msg);
    FREE(msg);
    fflush(log);
}
