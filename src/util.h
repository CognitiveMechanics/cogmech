
#ifndef CM_UTIL_H
#define CM_UTIL_H

#include <stdbool.h>


#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


char *cm_shift_arg (int *argc, char ***argv);
bool cm_str_ends_with(const char* str, const char* suffix);


#endif // CM_UTIL_H
