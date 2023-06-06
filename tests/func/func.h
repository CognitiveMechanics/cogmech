
#ifndef CM_FUNC_H
#define CM_FUNC_H

#include <stdlib.h>
#include <stdbool.h>

void cm_run_func_test (char *filename, char *buf_stdout, char *buf_stderr, size_t buf_size);
char *cm_shift_arg (int *argc, char ***argv);
void cm_overwrite_file (const char* filename, char* content);
bool cm_str_ends_with(const char* str, const char* suffix);


#endif // CM_FUNC_H
