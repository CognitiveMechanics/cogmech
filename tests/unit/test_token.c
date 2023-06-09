

#include "test_token.h"
#include <stdio.h>
#include <stdbool.h>

#include "unit.h"
#include "../../src/token.h"


bool test_cm_token_create (void)
{
	CMToken token = cm_token(
		"filename.cogm",
		0,
		1,
		CM_TOKEN_TYPE_WORD
	);

	token.value = cm_sv("value");

	if (token.type != CM_TOKEN_TYPE_WORD) {
		cm_test_error("Type doesn't match\n");
		return false;
	}

	if (token.loc.row != 0) {
		cm_test_error("Row doesn't match\n");
		return false;
	}

	if (token.loc.col != 1) {
		cm_test_error("Col doesn't match\n");
		return false;
	}

	if (strcmp(token.loc.filename, "filename.cogm") != 0) {
		cm_test_error("Filename doesn't match\n");
		return false;
	}

	if (! cm_sv_eq(token.value, cm_sv("value"))) {
		cm_test_error("Value doesn't match\n");
		return false;
	}

	return true;
}


bool test_cm_token_eq (void)
{
	CMToken token1 = cm_token(
		"filename.cogm",
		0,
		1,
		CM_TOKEN_TYPE_WORD
	);

	token1.value = cm_sv("test-value");

	CMToken token2 = cm_token(
		"filename.cogm",
		2,
		3,
		CM_TOKEN_TYPE_COMMA
	);

	CMToken token3 = token1;

	CMToken token4 = cm_token(
		"different-file.cogm",
		3,
		4,
		CM_TOKEN_TYPE_WORD
	);

	token4.value = cm_sv("test-value");

	if (cm_token_eq(token1, token2)) {
		cm_test_error("Eq states that two different tokens are the same\n");
		return false;
	}

	if (! cm_token_eq(token1, token1)) {
		cm_test_error("Eq states that the same tokens are different\n");
		return false;
	}

	if (! cm_token_eq(token1, token3)) {
		cm_test_error("Eq states that equivalent tokens are different\n");
		return false;
	}

	if (! cm_token_eq(token1, token4)) {
		cm_test_error("Eq states that same-valued tokens are different\n");
		return false;
	}

	return true;
}


void test_cm_token (void)
{
	printf("Loading token tests...\n");

	cm_add_test(test_cm_token_create);
	cm_add_test(test_cm_token_eq);
}
