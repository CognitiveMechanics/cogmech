
#ifndef CM_FILE_H
#define CM_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


char *cm_read_file_to_cstr (const char *filename);
void cm_overwrite_file (const char* filename, char* content);


#endif
