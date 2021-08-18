#include "main.h"

void free_multiple(int argc, ...)
{
    va_list ap;
    void *curr_ptr;

    va_start(ap, argc);
    for (int i = 0; i < argc; i++)
    {
        curr_ptr = va_arg(ap, void *);
        FREE(curr_ptr);
    }

    va_end(ap);
}

char *get_ext(char *file_name)
{
    char *e = strrchr(file_name, '.');
    if (e == NULL)
        e = "";

    char *f = (char *)malloc(strlen(e) * sizeof(char) + 1);
    strcpy(f, e);

    //Remove first character
    if (e != NULL)
        memmove(&f[0], &f[1], strlen(f));
    return f;
}

void recursive_mkdir(const char *dir)
{
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/')
        {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }

    if (mkdir(tmp, 0755) == -1 && errno != EEXIST)
        logger(ERROR, "Error while creating directory: \"%s\" | %s", dir, strerror(errno));
}

char *get_home_dir()
{
    struct passwd *pw = getpwuid(getuid());
    return pw->pw_dir;
}
