
#include <stdio.h>
#include <stdbool.h>

#include "unit.h"
#include "test_tokenizer.h"
#include "../../src/tokenizer.h"
#include "../func/func.h"


bool test_cm_tokenize_file (void)
{
	for (size_t i = 0; i < ARRAY_LEN(CM_FUNC_TEST_FILES); i++) {
		CMTokenList list = cm_tokenize_file(CM_FUNC_TEST_FILES[i]);

		if (cm_tokenlist_len(list) == 0) {
			cm_tokenlist_free(&list);
			cm_test_error("No tokens read\n");
			return false;
		}

		if (i == 0 && cm_tokenlist_len(list) != 15) {
			cm_tokenlist_free(&list);
			cm_test_error("Invalid number of tokens read\n");
			return false;
		}

		cm_tokenlist_free(&list);
	}

	return true;
}


void test_cm_tokenizer (void)
{
	printf("Loading tokenizer tests...\n");

	cm_add_test(test_cm_tokenize_file);
}
