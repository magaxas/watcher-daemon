#pragma once

#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define FREE(...) free_multiple(NARGS(__VA_ARGS__) - 1, __VA_ARGS__)

void free_multiple(int argc, ...);
char *get_ext(char *file_name);
void recursive_mkdir(const char *dir);
char *get_home_dir();
