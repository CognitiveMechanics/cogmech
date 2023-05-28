#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>

#include "tokenize.h"


#define USAGE_BUFFER_SIZE 2048
char cm_usage_buffer[USAGE_BUFFER_SIZE] = {0};


const char* cm_get_usage ()
{
	char usage_file_path[256] = {0};

    snprintf(usage_file_path, sizeof(usage_file_path), "%s%s", dirname(__FILE__), "/../usage.txt");
	FILE * usage_file = fopen(usage_file_path, "r");

	assert(usage_file && "Unable to open usage.txt\n");

	size_t bytes_read = fread(cm_usage_buffer, sizeof(cm_usage_buffer), 1, usage_file);

	assert(usage_file && "Zero bytes read from usage.txt\n");

	return (const char *) cm_usage_buffer;
}


void cm_print_usage ()
{
	const char *usage = cm_get_usage();
	printf("%s\n", usage);
}


int main (int argc, const char **argv)
{
	printf("%s\n", cm_trim_left(cm_sv("\ttest1"), "").data);
	printf("%s\n", cm_trim_left_ws(cm_sv("\ttest2")).data);
	return 0;
}
