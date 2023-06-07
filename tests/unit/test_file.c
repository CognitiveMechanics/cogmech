

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "unit.h"
#include "test_file.h"
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


bool test_cm_overwrite_file (void)
{
	char *contents1 = "test1";
	char *contents2 = "test2";
	const char *path = "/tmp/cm_test_file";

	cm_overwrite_file(path, contents1);
	char *str1 = cm_read_file_to_cstr(path);

	cm_overwrite_file(path, contents2);
	char *str2 = cm_read_file_to_cstr(path);

	if (strcmp(str1, contents1) != 0) {
		cm_test_error("failed to write file\n");
		return false;
	}

	if (strcmp(str2, contents2) != 0) {
		cm_test_error("failed to overwrite file\n");
		return false;
	}

	return true;
}


void test_cm_file (void)
{
	printf("Loading file tests...\n");

	cm_add_test(test_cm_read_file_to_cstr);
	cm_add_test(test_cm_overwrite_file);
}
