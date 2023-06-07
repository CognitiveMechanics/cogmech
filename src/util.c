
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"


char *cm_shift_arg (int *argc, char ***argv)
{
	assert((*argc) > 0);

	char *value = (*argv)[0];

	(*argc) -= 1;
	(*argv) += 1;

	return value;
}


bool cm_str_ends_with(const char* str, const char* suffix)
{
	size_t str_length = strlen(str);
	size_t suffix_length = strlen(suffix);

	if (suffix_length > str_length) {
		return false;
	}

	const char* str_end = str + (str_length - suffix_length);

	return strcmp(str_end, suffix) == 0;
}
