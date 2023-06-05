
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "file.h"
#include "util.h"
#include "stringview.h"
#include "tokenizer.h"


const char *CM_TOKEN_TYPES_READABLE[CM_TOKEN_TYPE_COUNT] = {
	"CM_TOKEN_TYPE_UNKNOWN",
	"CM_TOKEN_TYPE_WORD",
	"CM_TOKEN_TYPE_INT",
	"CM_TOKEN_TYPE_QUOTED",
	"CM_TOKEN_TYPE_ENDL",
	"CM_TOKEN_TYPE_LT",
	"CM_TOKEN_TYPE_GT",
	"CM_TOKEN_TYPE_COMMA",
	"CM_TOKEN_TYPE_COLON_EQ",
	"CM_TOKEN_TYPE_COLON",
	"CM_TOKEN_TYPE_PROXY",
	"CM_TOKEN_TYPE_DOT_PROXY",
	"CM_TOKEN_TYPE_SQ_BRACKET_IN",
	"CM_TOKEN_TYPE_SQ_BRACKET_OUT",
	"CM_TOKEN_TYPE_PAREN_IN",
	"CM_TOKEN_TYPE_PAREN_OUT",
	"CM_TOKEN_TYPE_PERCENT",
	"CM_TOKEN_TYPE_TRUE",
	"CM_TOKEN_TYPE_NULL",
	"CM_TOKEN_TYPE_DOT",
	"CM_TOKEN_TYPE_HASH",
	"CM_TOKEN_TYPE_S_ARROW",
	"CM_TOKEN_TYPE_D_ARROW",
	"CM_TOKEN_TYPE_PLUS",
	"CM_TOKEN_TYPE_MINUS",
};


const char *CM_TOKEN_TYPE_SYMBOLS[CM_TOKEN_TYPE_COUNT] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"<",
	">",
	",",
	":=",
	":",
	"[]",
	"[*]",
	"[",
	"]",
	"(",
	")",
	"%",
	"true",
	"null",
	"*",
	"#",
	"->",
	"=>",
	"+",
	"-",
};


const char *cm_readable_token_type (CMTokenType type)
{
	return CM_TOKEN_TYPES_READABLE[type];
}


const char *cm_token_type_symbol (CMTokenType type)
{
	return CM_TOKEN_TYPE_SYMBOLS[type];
}


void cm_syntax_error (CMToken token, const char *message)
{
	fprintf(stderr, "FAILURE %s:%zu:%zu: %s: got %s",
		token.loc.filename, token.loc.row + 1, token.loc.col + 1,
		message, cm_readable_token_type(token.type));

	exit(CM_ERROR_EXIT_SYNTAX);
}


CMToken cm_token (const char * filename, size_t row, size_t col, CMTokenType type)
{
	return (CMToken) {
		.loc = (CMLoc) {
			.filename = filename,
			.row = row,
			.col = col,
		},
		.type = type,
		.value = CM_SV_NULL
	};
}


void cm_print_token (CMToken token)
{
	printf(
		"Token {\n"
		"  .type = %s\n"
		"  .value = %.*s\n"
		"  .loc = {\n"
		"    .filename = %s\n"
		"    .row = %zu\n"
		"    .col = %zu\n"
		"  }\n"
		"}\n",
		cm_readable_token_type(token.type),
		(int) token.value.len,
		token.value.data,
		token.loc.filename,
		token.loc.row,
		token.loc.col
	);
}


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


bool cm_token_eq (CMToken token1, CMToken token2)
{
	if (token1.type != token2.type) {
		return false;
	}

	if (! cm_sv_eq(token1.value, token2.value)) {
		return false;
	}

	return true;
}


CMToken cm_tokenlist_get (CMTokenList list, size_t i)
{
	if (list.len < 1 || i > list.len - 1) {
		assert(false && "Attempted to access unset index with cm_tokenlist_get");
	}

	return list.tokens[list.cur + i];
}


CMToken cm_tokenlist_first (CMTokenList list)
{
	if (list.len < 1) {
		assert(false && "Attempted to access empty list with cm_tokenlist_first");
	}

	return cm_tokenlist_get(list, 0);
}


CMToken cm_tokenlist_last (CMTokenList list)
{
	if (list.len < 1) {
		assert(false && "Attempted to access empty list with cm_tokenlist_last");
	}

	return cm_tokenlist_get(list, list.len - 1);
}


#define cm_tokenlist_like(list, types) _cm_tokenlist_like(list, types, ARRAY_LEN(types))

bool _cm_tokenlist_like (CMTokenList list, const CMTokenType types[], size_t types_len)
{
	if (list.len < types_len) {
		return false;
	}

	size_t token_i = 0;

	for (size_t i = 0; i < types_len; i += 1) {
		while (cm_tokenlist_get(list, token_i).type == CM_TOKEN_TYPE_ENDL) {
			token_i += 1;

			if (token_i >= list.len) {
				return false;
			}
		}

		if (cm_tokenlist_get(list, token_i).type != types[i]) {
			return false;
		}

		token_i += 1;

		if (token_i >= list.len) {
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
	if (list->len < 1) {
		assert(false && "Attempted to shift empty list");
	}

	CMToken token = cm_tokenlist_first(*list);

	list->cur += 1;
	list->len -= 1;

	return token;
}


CMToken cm_tokenlist_expect (CMTokenList *list, CMTokenType type)
{
	if (list->len < 1) {
		assert(false && "cm_tokenlist_expect requires at least one value in the list");
	}

	CMToken token = cm_tokenlist_shift(list);

	if (token.type != type) {
		char message[128];
		const char *type_name = cm_readable_token_type(type);

		snprintf(message, sizeof(message), "Expected %s", type_name);
		cm_syntax_error(token, message);
	}

	return token;
}


bool cm_tokenlist_empty (CMTokenList list)
{
	return list.len == 0;
}


void cm_tokenlist_skip_endl (CMTokenList *list)
{
	while (! cm_tokenlist_empty(*list) && cm_tokenlist_first(*list).type == CM_TOKEN_TYPE_ENDL) {
		cm_tokenlist_shift(list);
	}
}


void cm_print_tokenlist (CMTokenList list)
{
	for (size_t i = 0; i < list.len; i++) {
		cm_print_token(cm_tokenlist_get(list, i));
	}
}


void cm_tokenlist_realloc (CMTokenList *list)
{
	list->cap = list->cap + CM_TOKEN_LIST_BLOCK_SIZE;
	list->tokens = realloc(list->tokens, list->cap);

	assert(list->tokens != NULL);
}


void cm_tokenlist_append (CMTokenList *list, CMToken token)
{
	if (list->len + list->cur + 1 > list->cap) {
		cm_tokenlist_realloc(list);
	}

	list->tokens[list->cur + list->len] = token;
	list->len = list->len + 1;
}


void cm_tokenlist_clear (CMTokenList *list)
{
	list->len = 0;
}


void cm_tokenlist_free (CMTokenList *list)
{
	free(list->tokens);
	list->len = 0;
	list->cap = 0;
	list->cur = 0;
}


bool cm_is_word (char c)
{
	return (bool) isalnum(c) || c == '_';
}


bool cm_is_num (char c)
{
	return (bool) isnumber(c) || c == '_';
}


CMTokenList cm_tokenize (const char *filename, CMStringView sv)
{
	assert(CM_TOKEN_TYPE_COUNT == 25);

	size_t row = 0;
	size_t col = 0;

	CMTokenList list = cm_tokenlist();

	while (! cm_sv_empty(sv)) {
		size_t trimmed = cm_trim_left(&sv, " \t");
		bool is_punctuation = false;
		col += trimmed;

		for (size_t type = 0; type < CM_TOKEN_TYPE_COUNT; type++) {
			const char* symbol = cm_token_type_symbol(type);

			if (symbol != NULL) {
				if (cm_starts_with(sv, cm_sv(symbol))) {
					size_t symbol_len = strlen(symbol);

					cm_tokenlist_append(
						&list,
						cm_token(filename, row, col, type)
					);

					cm_chop_left_len(&sv, symbol_len);
					col += symbol_len;
					is_punctuation = true;
					break;
				}
			}
		}

		if (is_punctuation) {
			continue;
		}

		if (cm_starts_with(sv, cm_sv("\""))) {
			CMToken quoted = cm_token(filename, row, col, CM_TOKEN_TYPE_QUOTED);
			size_t curr = 1;
			bool terminated = false;

			while (curr < sv.len) {
				if (sv.data[curr] == '"') {
					quoted.value = cm_chop_left_len(&sv, curr + 1);
					terminated = true;
					break;
				}

				curr += 1;
			}

			if (! terminated) {
				cm_syntax_error(quoted, "Unterminated quote");
			}

			col += quoted.value.len;

			// remove quotes
			quoted.value.data = quoted.value.data + 1;
			quoted.value.len = quoted.value.len - 2;

			cm_tokenlist_append(&list, quoted);

		} else if (isalpha(sv.data[0])) {
			CMToken word = cm_token(filename, row, col, CM_TOKEN_TYPE_WORD);
			word.value = cm_chop_left_while(&sv, cm_is_word);

			cm_tokenlist_append(&list, word);
			col += word.value.len;

		} else if (isnumber(sv.data[0])) {
			CMToken word = cm_token(filename, row, col, CM_TOKEN_TYPE_INT);
			word.value = cm_chop_left_while(&sv, cm_is_num);

			cm_tokenlist_append(&list, word);
			col += word.value.len;

		} else if (cm_in_chars(sv.data[0], "\n")) {
			CMToken word = cm_token(filename, row, col, CM_TOKEN_TYPE_ENDL);
			cm_chop_left_len(&sv, 1);

			cm_tokenlist_append(&list, word);
			row += 1;
			col = 0;

		} else if (cm_in_chars(sv.data[0], "\r")) {
			// just skip carriage return
			cm_trim_left(&sv, "\r");

		} else {
			CMToken unknown = cm_token(filename, row, col, CM_TOKEN_TYPE_UNKNOWN);
			cm_syntax_error(unknown, "Invalid token");
		}
	}

	return list;
}


CMTokenList cm_tokenize_file (const char *filename)
{
	return cm_tokenize(
		filename,
		cm_sv(cm_read_file_to_cstr(filename))
	);
}