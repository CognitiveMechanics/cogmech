

#include "file.h"


char *cm_read_file_to_cstr (const char *filename)
{
	size_t length;
	FILE *f = fopen(filename, "rb");
	char *str;

	assert(f && "File could not be opened for reading");

	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);

	str = calloc(length + 1, sizeof(char));

	assert(str && "String could not be allocated");

	fread(str, 1, length, f);
	str[length] = '\0';

	fclose(f);

	return str;
}