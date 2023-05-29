
#include <stdio.h>
#include <stdbool.h>

#include "../../src/tokenizer.h"


bool test_cm_token ()
{
	CMToken token = cm_token(
		"filename.cogm",
		0,
		1,
		CM_TOKEN_TYPE_WORD
	);

	token.value = cm_sv("value");

	if (token.type != CM_TOKEN_TYPE_WORD) {
		cm_test_error("test_cm_token: Type doesn't match\n");
		return false;
	}

	if (token.loc.row != 0) {
		cm_test_error("test_cm_token: Row doesn't match\n");
		return false;
	}

	if (token.loc.col != 1) {
		cm_test_error("test_cm_token: Col doesn't match\n");
		return false;
	}

	if (strcmp(token.loc.filename, "filename.cogm") != 0) {
		cm_test_error("test_cm_token: Filename doesn't match\n");
		return false;
	}

	if (! cm_sv_eq(token.value, cm_sv("value"))) {
		cm_test_error("test_cm_token: Value doesn't match\n");
		return false;
	}

	return true;
}


bool test_cm_tokenlist ()
{
	CMTokenList list = cm_tokenlist();

	if (list.len != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenlist: List not empty\n");
		return false;
	}

	cm_tokenlist_free(&list);

	return true;
}


bool test_cm_tokenlist_append_clear ()
{
	CMTokenList list = cm_tokenlist();

	CMToken token1 = cm_token(
		"filename.cogm",
		0,
		1,
		CM_TOKEN_TYPE_COLON
	);

	CMToken token2 = cm_token(
		"filename.cogm",
		2,
		3,
		CM_TOKEN_TYPE_COMMA
	);

	cm_tokenlist_append(&list, token1);
	cm_tokenlist_append(&list, token2);

	if (list.len != 2) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenlist_append: List contains incorrect number of elements\n");
		return false;
	}

	if (list.tokens[0].type != CM_TOKEN_TYPE_COLON) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenlist_append: Incorrect type for first token\n");
		return false;
	}

	if (list.tokens[1].type != CM_TOKEN_TYPE_COMMA) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenlist_append: Incorrect type for first token\n");
		return false;
	}

	cm_tokenlist_clear(&list);

	if (list.len != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenlist_append: List did not clear\n");
		return false;
	}

	cm_tokenlist_free(&list);

	return true;
}


bool test_cm_tokenize_file ()
{
	CMTokenList list = cm_tokenize_file("tests/cogm/00-hello.cogm");

	if (list.len == 0) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenize_file: No tokens read\n");
		return false;
	}

	if (list.len != 15) {
		cm_tokenlist_free(&list);
		cm_test_error("test_cm_tokenize_file: Invalid number of tokens read\n");
		return false;
	}

	cm_tokenlist_free(&list);

	return true;
}


void test_cm_tokenizer ()
{
	printf("Testing tokenizer...\n");

	cm_add_test(test_cm_token);
	cm_add_test(test_cm_tokenlist);
	cm_add_test(test_cm_tokenlist_append_clear);
	cm_add_test(test_cm_tokenize_file);
}