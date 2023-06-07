
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
	"CM_TOKEN_TYPE_INCLUDE",
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
	"@include",
};


const char *cm_readable_token_type (CMTokenType type)
{
	return CM_TOKEN_TYPES_READABLE[type];
}


const char *cm_token_type_symbol (CMTokenType type)
{
	return CM_TOKEN_TYPE_SYMBOLS[type];
}


void  cm_syntax_error (CMToken token, const char *message)
{
	fflush(stdout);

	if (! cm_sv_eq(token.value, CM_SV_NULL)) {
		fprintf(stderr, "ERROR %s:%zu:%zu: %s(%.*s): %s",
			token.loc.filename, token.loc.row + 1, token.loc.col + 1,
			 cm_readable_token_type(token.type),
			(int) token.value.len, token.value.data,
			message
		);
	} else {
		fprintf(stderr, "ERROR %s:%zu:%zu: %s: %s",
			token.loc.filename, token.loc.row + 1, token.loc.col + 1,
			cm_readable_token_type(token.type),
			message
		);
	}

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


void cm_print_token_indented (CMToken token, int indent_level, int num_spaces)
{
	printf(
		"%*sToken {\n"
		"%*s.type = %s\n"
		"%*s.value = %.*s\n"
		"%*s.loc = {\n"
		"%*s.filename = %s\n"
		"%*s.row = %zu\n"
		"%*s.col = %zu\n"
		"%*s}\n"
		"%*s}\n",

		indent_level * num_spaces,
		"",
		(indent_level + 1) * num_spaces,
		"",

		cm_readable_token_type(token.type),

		(indent_level + 1) * num_spaces,
		"",

		(int) token.value.len,
		token.value.data,

		(indent_level + 1) * num_spaces,
		"",
		(indent_level + 2) * num_spaces,
		"",

		token.loc.filename,

		(indent_level + 2) * num_spaces,
		"",

		token.loc.row,

		(indent_level + 2) * num_spaces,
		"",

		token.loc.col,

		(indent_level + 1) * num_spaces,
		"",
		indent_level * num_spaces,
		""
	);
}


void cm_print_token (CMToken token)
{
	cm_print_token_indented(token, 0, 2);
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


CMToken cm_tokenlist_expect (CMTokenList *list, CMTokenType type)
{
	if (cm_tokenlist_len(*list) < 1) {
		CMToken last = list->tokens[list->cur - 1];
		cm_syntax_error(last, "Unexpected end of tokens");
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
	list->tokens = realloc(list->tokens, list->cap);

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


bool cm_is_word (char c)
{
	return (bool) isalnum(c) || c == '_';
}


bool cm_is_not_space (char c)
{
	return (bool) ! isspace(c);
}


bool cm_is_num (char c)
{
	return (bool) isnumber(c) || c == '_';
}


CMTokenList cm_tokenize (const char *filename, CMStringView sv)
{
	assert(CM_TOKEN_TYPE_COUNT == 26);

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

			col += quoted.value.len;

			if (! terminated) {
				quoted.value = cm_chop_left_len(&sv, curr);
				cm_syntax_error(quoted, "Unterminated quote");
			}

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
			unknown.value = cm_chop_left_while(&sv, cm_is_not_space);
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
