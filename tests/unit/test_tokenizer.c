
#include <stdio.h>
#include <stdbool.h>

#include "../../src/tokenizer.h"


bool test_cm_tokenize_file ()
{
	CMTokenList list = cm_tokenize_file("tests/cogm/00-hello.cogm");

	if (list.len == 0) {
		cm_test_error("test_cm_tokenize_file: No tokens read\n");
		return false;
	}

	if (list.len != 15) {
		cm_test_error("test_cm_tokenize_file: Invalid number of tokens read\n");
		return false;
	}

	return true;
}


void test_cm_tokenizer ()
{
	printf("Testing tokenizer...\n");

	cm_add_test(test_cm_tokenize_file);
}