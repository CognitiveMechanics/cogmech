
#include <stdio.h>
#include <stdbool.h>

#include "unit.h"
#include "test_tokenizer.h"
#include "../../src/tokenizer.h"
#include "../func/func.h"


bool test_cm_tokenize_quote (void)
{
	CMLoc loc = cm_loc("test.cogm", 0, 0);
	CMStringView sv = cm_sv("\"Some_Word1\" Another");

	CMToken token = cm_tokenize_quote(&loc, &sv);

	if (token.type != CM_TOKEN_TYPE_QUOTED) {
		cm_test_error("Invalid token type\n");
		return false;
	}

	if (! cm_sv_eq(token.value, cm_sv("Some_Word1"))) {
		cm_test_error("Invalid token value\n");
		return false;
	}

	if (loc.col != 12 || loc.row != 0) {
		cm_test_error("Invalid loc\n");
		return false;
	}

	return true;
}


bool test_cm_tokenize_int (void)
{
	CMLoc loc = cm_loc("test.cogm", 0, 0);
	CMStringView sv = cm_sv("12345 Another");

	CMToken token = cm_tokenize_int(&loc, &sv);

	if (token.type != CM_TOKEN_TYPE_INT) {
		cm_test_error("Invalid token type\n");
		return false;
	}

	if (! cm_sv_eq(token.value, cm_sv("12345"))) {
		cm_test_error("Invalid token value\n");
		return false;
	}

	if (loc.col != 5 || loc.row != 0) {
		cm_test_error("Invalid loc\n");
		return false;
	}

	return true;
}


bool test_cm_tokenize_file (void)
{
	for (size_t i = 0; i < ARRAY_LEN(CM_FUNC_TEST_FILES); i++) {
		CMContext context = cm_context();
		CMTokenList list = cm_tokenize_file(&context, CM_FUNC_TEST_FILES[i]);

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

	cm_add_test(test_cm_tokenize_quote);
	cm_add_test(test_cm_tokenize_int);
	cm_add_test(test_cm_tokenize_file);
}
