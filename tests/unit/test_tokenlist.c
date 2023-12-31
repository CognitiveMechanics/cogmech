
#include <stdio.h>
#include <stdbool.h>

#include "unit.h"
#include "../../src/tokenlist.h"
#include "test_tokenlist.h"


bool test_cm_tokenlist_create (void)
{
	CMTokenList list = cm_tokenlist();

	if (cm_tokenlist_len(list) != 0) {
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

	if (cm_tokenlist_len(list) != 2) {
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

	if (cm_tokenlist_len(list) != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("List did not clear\n");
		return false;
	}

	cm_tokenlist_free(&list);

	return true;
}


bool test_cm_tokenlist_len (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token = cm_token(
		"filename.cogm",
		0,
		1,
		CM_TOKEN_TYPE_COLON
	);

	if (cm_tokenlist_len(list) != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid tokenlist len\n");
		return false;
	}

	cm_tokenlist_append(&list, token);

	if (cm_tokenlist_len(list) != 1) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid tokenlist len\n");
		return false;
	}

	cm_tokenlist_append(&list, token);

	if (cm_tokenlist_len(list) != 2) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid tokenlist len\n");
		return false;
	}

	cm_tokenlist_shift(&list);

	if (cm_tokenlist_len(list) != 1) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid tokenlist len\n");
		return false;
	}

	cm_tokenlist_shift(&list);

	if (cm_tokenlist_len(list) != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid tokenlist len\n");
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

	if (cm_tokenlist_len(list) != 2) {
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

	if (cm_tokenlist_len(list) != 0) {
		cm_tokenlist_free(&list);
		cm_test_error("List did not clear\n");
		return false;
	}

	cm_tokenlist_free(&list);
	return true;
}


bool test_cm_tokenlist_clone (void)
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

	CMTokenList clone = cm_tokenlist_clone(list);

	if (clone.len != cm_tokenlist_len(list)) {
		cm_tokenlist_free(&list);
		cm_tokenlist_free(&clone);
		cm_test_error("List lengths are not equal\n");
		return false;
	}

	if (clone.cur != list.cur) {
		cm_tokenlist_free(&list);
		cm_tokenlist_free(&clone);
		cm_test_error("List cursors are not equal\n");
		return false;
	}

	if (clone.cap != list.cap) {
		cm_tokenlist_free(&list);
		cm_tokenlist_free(&clone);
		cm_test_error("List capacities are not equal\n");
		return false;
	}

	if (! cm_token_eq(cm_tokenlist_get(list, 0), cm_tokenlist_get(clone, 0))) {
		cm_tokenlist_free(&list);
		cm_tokenlist_free(&clone);
		cm_test_error("First elements not equal\n");
		return false;
	}

	if (! cm_token_eq(cm_tokenlist_get(list, 1), cm_tokenlist_get(clone, 1))) {
		cm_tokenlist_free(&list);
		cm_tokenlist_free(&clone);
		cm_test_error("Second elements not equal\n");
		return false;
	}

	if (list.tokens == clone.tokens) {
		cm_tokenlist_free(&list);
		cm_tokenlist_free(&clone);
		cm_test_error("Tokens pointers are the same\n");
		return false;
	}

	cm_tokenlist_free(&list);
	cm_tokenlist_free(&clone);
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

	CMToken endl = cm_token(
		"filename.cogm",
		0,
		2,
		CM_TOKEN_TYPE_ENDL
	);

	CMToken token3 = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	token3.value = cm_sv("b");

	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, token1);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, token2);
	cm_tokenlist_append(&list, endl);
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


bool test_cm_tokenlist_first_like (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken endl = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_ENDL
	);

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

	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, token1);
	cm_tokenlist_append(&list, token2);
	cm_tokenlist_append(&list, token3);

	if (! cm_tokenlist_first_like(list, CM_TOKEN_TYPE_WORD)) {
		cm_tokenlist_free(&list);
		cm_test_error("should be CM_TOKEN_TYPE_WORD");
		return false;
	}

	if (cm_tokenlist_first_like(list, CM_TOKEN_TYPE_COLON_EQ)) {
		cm_tokenlist_free(&list);
		cm_test_error("should not be CM_TOKEN_TYPE_COLON_EQ");
		return false;
	}

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

	if (cm_tokenlist_len(list) != 1) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid list length after first shift\n");
		return false;
	}

	if (cm_tokenlist_first(list).type != CM_TOKEN_TYPE_COLON_EQ) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid list element token type after shift\n");
		return false;
	}

	CMToken shifted2 = cm_tokenlist_expect(&list, CM_TOKEN_TYPE_COLON_EQ, NULL);

	if (shifted2.type != CM_TOKEN_TYPE_COLON_EQ) {
		cm_tokenlist_free(&list);
		cm_test_error("Invalid token type after second shift\n");
		return false;
	}

	if (cm_tokenlist_len(list) != 0) {
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


void test_cm_tokenlist (void)
{
	printf("Loading tokenlist tests...\n");

	cm_add_test(test_cm_tokenlist_create);
	cm_add_test(test_cm_tokenlist_len);
	cm_add_test(test_cm_tokenlist_clone);
	cm_add_test(test_cm_tokenlist_like);
	cm_add_test(test_cm_tokenlist_first_like);
	cm_add_test(test_cm_tokenlist_get);
	cm_add_test(test_cm_tokenlist_append_clear);
	cm_add_test(test_cm_tokenlist_shift_expect);
	cm_add_test(test_cm_tokenlist_empty);
}
