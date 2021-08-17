#pragma once

#define FREE(ptr) if (ptr != NULL) { free(ptr), ptr = NULL; }
#define MFREE(argc, ...) free_multiple(argc, __VA_ARGS__)

void free_multiple(int argc, ...);
char *get_ext(char *file_name);
