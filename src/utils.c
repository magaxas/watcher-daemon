#include "main.h"

void free_multiple(int argc, ...)
{
    va_list ap;
    void *curr_ptr;

    va_start(ap, argc);
    for (int i = 0; i < argc; i++) {
        curr_ptr = va_arg(ap, void *);

        if (curr_ptr != NULL) {
            free(curr_ptr);
            curr_ptr = NULL;
        }
    }

    va_end(ap);
}

char *get_ext(char *file_name)
{
    char *e = strrchr(file_name, '.');
    if (e == NULL) {
        e = "";
    }

    char *f = (char *) malloc(strlen(e) * sizeof(char) + 1);
    if (f == NULL) return NULL;
    strcpy(f, e);

    //Remove first character
    if (e != NULL) {
        memmove(&f[0], &f[1], strlen(f));
    }

    return f;
}

void recursive_mkdir(const char *dir)
{
    int err = 0;
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);

    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            err = mkdir(tmp, 0755);
            *p = '/';
        }
    }

    err = mkdir(tmp, 0755);
    if (err == -1 && errno != EEXIST) {
        logger(
            ERROR,
            "Error while creating directory: \"%s\" | %s",
            dir,
            strerror(errno)
        );
        perror("Error while creating directory");
    }
}

char *get_home_dir()
{
    char *home = (char *) calloc(PATH_MAX, 1);
    if (home == NULL) NULL;

    char *user = getenv("SUDO_USER");
    if (user != NULL) {
        strcat(home, "/home/");
        strcat(home, user);
    } else {
        struct passwd *pw = getpwuid(getuid());
        strcat(home, pw->pw_dir);
    }

    return home;
}
