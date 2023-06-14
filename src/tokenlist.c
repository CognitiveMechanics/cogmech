
#include "tokenlist.h"

void cm_syntax_error (CMToken token, const char *message);


CMTokenList cm_tokenlist (void)
{
	CMTokenList list;

	list.tokens = calloc(CM_TOKEN_LIST_BLOCK_SIZE, sizeof(CMToken));
	list.len = 0;
	list.cap = CM_TOKEN_LIST_BLOCK_SIZE;
	list.cur = 0;

	return list;
}


CMTokenList cm_tokenlist_clone (CMTokenList list)
{
	CMTokenList clone;

	clone.len = list.len;
	clone.cap = list.cap;
	clone.cur = list.cur;

	clone.tokens = calloc(list.cap, sizeof(CMToken));
	memcpy(clone.tokens, list.tokens, list.len * sizeof(CMToken));

	return clone;
}


CMToken cm_tokenlist_get (CMTokenList list, size_t i)
{
	if (cm_tokenlist_len(list) < 1 || i > cm_tokenlist_len(list) - 1) {
		assert(false && "Attempted to access unset index with cm_tokenlist_get");
	}

	return list.tokens[list.cur + i];
}


CMToken cm_tokenlist_first (CMTokenList list)
{
	if (cm_tokenlist_len(list) < 1) {
		assert(false && "Attempted to access empty list with cm_tokenlist_first");
	}

	return cm_tokenlist_get(list, 0);
}


CMToken cm_tokenlist_last (CMTokenList list)
{
	if (cm_tokenlist_len(list) < 1) {
		assert(false && "Attempted to access empty list with cm_tokenlist_last");
	}

	return cm_tokenlist_get(list, cm_tokenlist_len(list) - 1);
}


size_t cm_tokenlist_len (CMTokenList list)
{
	return list.len - list.cur;
}


#define cm_tokenlist_like(list, types) _cm_tokenlist_like(list, types, ARRAY_LEN(types))

bool _cm_tokenlist_like (CMTokenList list, const CMTokenType types[], size_t types_len)
{
	if (cm_tokenlist_len(list) < types_len) {
		return false;
	}

	size_t token_i = 0;

	for (size_t i = 0; i < types_len; i += 1) {
		while (cm_tokenlist_get(list, token_i).type == CM_TOKEN_TYPE_ENDL) {
			token_i += 1;

			if (token_i >= cm_tokenlist_len(list)) {
				return false;
			}
		}

		if (cm_tokenlist_get(list, token_i).type != types[i]) {
			return false;
		}

		if (i == types_len - 1) {
			break;
		}

		token_i += 1;

		if (token_i >= cm_tokenlist_len(list)) {
			return false;
		}
	}

	return true;
}

bool cm_tokenlist_first_like (CMTokenList list, CMTokenType type)
{
	if (list.len < 1) {
		return false;
	}

	CMToken next = cm_tokenlist_first(list);
	size_t i = 0;

	while (next.type == CM_TOKEN_TYPE_ENDL) {
		i += 1;
		next = cm_tokenlist_get(list, i);
	}

	return next.type == type;
}


CMToken cm_tokenlist_shift (CMTokenList *list)
{
	if (cm_tokenlist_len(*list) < 1) {
		assert(false && "Attempted to shift empty list");
	}

	CMToken token = cm_tokenlist_first(*list);

	list->cur += 1;

	return token;
}


CMToken cm_tokenlist_expect (CMTokenList *list, CMTokenType type, const char *message)
{
	if (cm_tokenlist_len(*list) < 1) {
		CMToken last = list->tokens[list->cur - 1];
		cm_syntax_error(last, "Unexpected end of tokens");
	}

	CMToken token = cm_tokenlist_shift(list);

	if (token.type != type) {
		char full_message[128];
		const char *type_name = cm_readable_token_type(type);

		if (message != NULL) {
			snprintf(full_message, sizeof(full_message), "Expected %s: %s", type_name, message);
		} else {
			snprintf(full_message, sizeof(full_message), "Expected %s", type_name);
		}

		cm_syntax_error(token, full_message);
	}

	return token;
}


bool cm_tokenlist_empty (CMTokenList list)
{
	return cm_tokenlist_len(list) == 0;
}


void cm_tokenlist_skip_endl (CMTokenList *list)
{
	while (! cm_tokenlist_empty(*list) && cm_tokenlist_first(*list).type == CM_TOKEN_TYPE_ENDL) {
		cm_tokenlist_shift(list);
	}
}


void cm_print_tokenlist (CMTokenList list)
{
	for (size_t i = 0; i < cm_tokenlist_len(list); i++) {
		cm_print_token(cm_tokenlist_get(list, i));
	}
}


void cm_tokenlist_realloc (CMTokenList *list)
{
	list->cap = list->cap + CM_TOKEN_LIST_BLOCK_SIZE;
	list->tokens = realloc(list->tokens, list->cap * sizeof(CMToken));

	assert(list->tokens != NULL);
}


void cm_tokenlist_append (CMTokenList *list, CMToken token)
{
	if (list->len + 1 > list->cap) {
		cm_tokenlist_realloc(list);
	}

	list->tokens[list->len] = token;
	list->len = list->len + 1;
}


void cm_tokenlist_clear (CMTokenList *list)
{
	list->cur = 0;
	list->len = 0;
}


void cm_tokenlist_free (CMTokenList *list)
{
	free(list->tokens);
	list->len = 0;
	list->cap = 0;
	list->cur = 0;
}
