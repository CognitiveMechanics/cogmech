
#ifndef CM_TEST_TOKENIZER_H
#define CM_TEST_TOKENIZER_H

#include <stdio.h>
#include <stdbool.h>

#include "../../src/tokenizer.h"


bool test_cm_token (void)
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


bool test_cm_tokenlist (void)
{
	CMTokenList list = cm_tokenlist();

	if (list.len != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("List not empty\n");
		return false;
	}

	cm_tokenlist_free(&list);

	return true;
}


bool test_cm_tokenlist_append_clear (void)
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
		cm_test_error("List contains incorrect number of elements\n");
		return false;
	}

	if (cm_tokenlist_first(list).type != CM_TOKEN_TYPE_COLON) {
		cm_tokenlist_free(&list);
		cm_test_error("Incorrect type for first token\n");
		return false;
	}

	if (cm_tokenlist_last(list).type != CM_TOKEN_TYPE_COMMA) {
		cm_tokenlist_free(&list);
		cm_test_error("Incorrect type for first token\n");
		return false;
	}

	cm_tokenlist_clear(&list);

	if (list.len != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("List did not clear\n");
		return false;
	}

	cm_tokenlist_free(&list);

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


bool test_cm_tokenlist_get (void)
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
		cm_test_error("List contains incorrect number of elements\n");
		return false;
	}

	if (cm_tokenlist_get(list, 0).type != CM_TOKEN_TYPE_COLON) {
		cm_tokenlist_free(&list);
		cm_test_error("Incorrect type for first token\n");
		return false;
	}

	if (! cm_token_eq(cm_tokenlist_get(list, 0), cm_tokenlist_first(list))) {
		cm_tokenlist_free(&list);
		cm_test_error("cm_tokenlist_first failed\n");
		return false;
	}

	if (cm_tokenlist_last(list).type != CM_TOKEN_TYPE_COMMA) {
		cm_tokenlist_free(&list);
		cm_test_error("Incorrect type for first token\n");
		return false;
	}

	cm_tokenlist_clear(&list);

	if (list.len != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("List did not clear\n");
		return false;
	}

	cm_tokenlist_free(&list);
	return true;
}


bool test_cm_tokenlist_like (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token1 = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	token1.value = cm_sv("a");

	CMToken token2 = cm_token(
		"filename.cogm",
		0,
		2,
		CM_TOKEN_TYPE_COLON_EQ
	);

	CMToken token3 = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	token3.value = cm_sv("b");

	cm_tokenlist_append(&list, token1);
	cm_tokenlist_append(&list, token2);
	cm_tokenlist_append(&list, token3);

	CMTokenType type_list1[] = {CM_TOKEN_TYPE_WORD, CM_TOKEN_TYPE_COLON_EQ};
	CMTokenType type_list2[] = {CM_TOKEN_TYPE_WORD, CM_TOKEN_TYPE_WORD};

	if (! cm_tokenlist_like(list, type_list1)) {
		cm_tokenlist_free(&list);
		cm_test_error("Equals test failed\n");
		return false;
	}

	if (cm_tokenlist_like(list, type_list2)) {
		cm_tokenlist_free(&list);
		cm_test_error("Equals test failed\n");
		return false;
	}

	cm_tokenlist_free(&list);
	return true;
}

bool test_cm_tokenlist_shift_expect (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token1 = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	token1.value = cm_sv("a");

	CMToken token2 = cm_token(
		"filename.cogm",
		0,
		2,
		CM_TOKEN_TYPE_COLON_EQ
	);

	cm_tokenlist_append(&list, token1);
	cm_tokenlist_append(&list, token2);

	CMToken shifted1 = cm_tokenlist_shift(&list);

	if (shifted1.type != CM_TOKEN_TYPE_WORD) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid token type after first shift\n");
		return false;
	}

	if (list.len != 1) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid list length after first shift\n");
		return false;
	}

	if (cm_tokenlist_first(list).type != CM_TOKEN_TYPE_COLON_EQ) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid list element token type after shift\n");
		return false;
	}

	CMToken shifted2 = cm_tokenlist_expect(&list, CM_TOKEN_TYPE_COLON_EQ);

	if (shifted2.type != CM_TOKEN_TYPE_COLON_EQ) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid token type after second shift\n");
		return false;
	}

	if (list.len != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid list length after second shift\n");
		return false;
	}

	cm_tokenlist_free(&list);
	return true;
}

bool test_cm_tokenlist_empty (void)
{
	CMTokenList list = cm_tokenlist();

	if (! cm_tokenlist_empty(list)) {
		cm_tokenlist_free(&list);
		cm_test_error("List is empty\n");
		return false;
	}

	CMToken token = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON_EQ
	);

	cm_tokenlist_append(&list, token);

	if (cm_tokenlist_empty(list)) {
		cm_tokenlist_free(&list);
		cm_test_error("List is not empty\n");
		return false;
	}

	cm_tokenlist_free(&list);
	return true;
}


bool test_cm_tokenize_file (void)
{
	CMTokenList list = cm_tokenize_file("../tests/cogm/00-hello.cogm");

	if (list.len == 0) {
		cm_tokenlist_free(&list);
		cm_test_error("No tokens read\n");
		return false;
	}

	if (list.len != 15) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid number of tokens read\n");
		return false;
	}

	cm_tokenlist_free(&list);

	return true;
}


void test_cm_tokenizer (void)
{
	printf("Loading tokenizer tests...\n");

	cm_add_test(test_cm_token);
	cm_add_test(test_cm_tokenlist);
	cm_add_test(test_cm_token_eq);
	cm_add_test(test_cm_tokenlist_like);
	cm_add_test(test_cm_tokenlist_get);
	cm_add_test(test_cm_tokenlist_append_clear);
	cm_add_test(test_cm_tokenlist_shift_expect);
	cm_add_test(test_cm_tokenlist_empty);
	cm_add_test(test_cm_tokenize_file);
}


#endif // CM_TEST_TOKENIZER_H
