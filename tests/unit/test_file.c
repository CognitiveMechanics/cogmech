
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "unit.h"
#include "../../src/file.h"


bool test_cm_read_file_to_cstr ()
{
	char *str = cm_read_file_to_cstr("USAGE.txt");

	if (strlen(str) <= 0) {
		cm_test_error("test_cm_read_file_to_cstr failed to read file\n");
		return false;
	}

	return true;
}


void test_cm_file ()
{
	printf("Testing files...\n");

	cm_add_test(test_cm_read_file_to_cstr);
}