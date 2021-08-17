#pragma once

#define FREE(ptr) if (ptr != NULL) { free(ptr), ptr = NULL; }
#define MFREE(argc, ...) free_multiple(argc, __VA_ARGS__)

extern void free_multiple(int argc, ...);
extern char *getExt(const char *file_name);
