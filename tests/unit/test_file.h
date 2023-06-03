
#ifndef CM_TEST_FILE_H
#define CM_TEST_FILE_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "unit.h"
#include "../../src/file.h"
#include "../../src/util.h"


bool test_cm_read_file_to_cstr (void)
{
	const char *contents = "test";
	const char *path = "/tmp/cm_test_file";

	FILE *file = fopen(path, "w+");
	fwrite(contents, sizeof(char), sizeof(contents), file);
	fclose(file);

	char *str = cm_read_file_to_cstr(path);

	if (strlen(str) != 4) {
		cm_test_error("test_cm_read_file_to_cstr failed to read file\n");
		return false;
	}

	return true;
}


void test_cm_file (void)
{
	printf("Loading file tests...\n");

	cm_add_test(test_cm_read_file_to_cstr);
}


#endif // CM_TEST_FILE_H
