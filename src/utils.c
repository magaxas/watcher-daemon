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

char *getExt(const char *file_name)
{
    char *e = strrchr(file_name, '.');
    if (e == NULL)
        e = "";
    return e;
}
