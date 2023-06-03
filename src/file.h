
#ifndef CM_FILE_H
#define CM_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


char *cm_read_file_to_cstr (const char *filename)
{
	size_t length;
	FILE *f = fopen(filename, "rb");
	char *str;

	assert(f && "File could not be opened for reading");

	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);

	str = malloc(length + 1);

	assert(str && "String could not be allocated");

	fread(str, 1, length, f);
	str[length] = '\0';

	fclose(f);

	return str;
}

#endif
