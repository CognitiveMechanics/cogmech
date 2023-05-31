
#ifndef CM_TOKENIZER_H
#define CM_TOKENIZER_H

#ifndef CM_TOKEN_LIST_BLOCK_SIZE
#define CM_TOKEN_LIST_BLOCK_SIZE 2048
#endif

#define CM_ERROR_EXIT_SYNTAX 2


#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "util.h"
#include "stringview.h"


typedef struct CMLoc
{
	const char * filename;
	size_t row;
	size_t col;
} CMLoc;


typedef enum CMTokenType
{
	CM_TOKEN_TYPE_UNKNOWN = 0,
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_QUOTED,
	CM_TOKEN_TYPE_LT,
	CM_TOKEN_TYPE_GT,
	CM_TOKEN_TYPE_COMMA,
	CM_TOKEN_TYPE_COLON_EQ,
	CM_TOKEN_TYPE_PAREN_IN,
	CM_TOKEN_TYPE_PAREN_OUT,
	CM_TOKEN_TYPE_S_ARRROW,
	CM_TOKEN_TYPE_D_ARROW,
	CM_TOKEN_TYPE_BRACKET_IN,
	CM_TOKEN_TYPE_BRACKET_OUT,
	CM_TOKEN_TYPE_SQBRACKET_IN,
	CM_TOKEN_TYPE_SQBRACKET_OUT,
	CM_TOKEN_TYPE_DOT,
	CM_TOKEN_TYPE_COLON,
	CM_TOKEN_TYPE_ENDL,
	CM_TOKEN_TYPE_PIPE,
	CM_TOKEN_TYPE_DOLLAR,
	CM_TOKEN_TYPE_BANG,
	CM_TOKEN_TYPE_QUESTION,
	CM_TOKEN_TYPE_COUNT,
} CMTokenType;


const char *CM_TOKEN_TYPES_READABLE[CM_TOKEN_TYPE_COUNT] = {
	"CM_TOKEN_TYPE_WORD",
	"CM_TOKEN_TYPE_QUOTED",
	"CM_TOKEN_TYPE_LT",
	"CM_TOKEN_TYPE_GT",
	"CM_TOKEN_TYPE_COMMA",
	"CM_TOKEN_TYPE_COLON_EQ",
	"CM_TOKEN_TYPE_PAREN_IN",
	"CM_TOKEN_TYPE_PAREN_OUT",
	"CM_TOKEN_TYPE_S_ARRROW",
	"CM_TOKEN_TYPE_D_ARROW",
	"CM_TOKEN_TYPE_BRACKET_IN",
	"CM_TOKEN_TYPE_BRACKET_OUT",
	"CM_TOKEN_TYPE_SQBRACKET_IN",
	"CM_TOKEN_TYPE_SQBRACKET_OUT",
	"CM_TOKEN_TYPE_DOT",
	"CM_TOKEN_TYPE_COLON",
	"CM_TOKEN_TYPE_ENDL",
	"CM_TOKEN_TYPE_PIPE",
	"CM_TOKEN_TYPE_DOLLAR",
	"CM_TOKEN_TYPE_BANG",
	"CM_TOKEN_TYPE_QUESTION",
};


const char *CM_TOKEN_TYPE_SYMBOLS[CM_TOKEN_TYPE_COUNT] = {
	NULL,
	NULL,
	"<",
	">",
	",",
	":=",
	"(",
	")",
	"->",
	"=>",
	"{",
	"}",
	"[",
	"]",
	".",
	":",
	NULL,
	"|",
	"$",
	"!",
	"?",
};


typedef struct CMToken
{
	CMLoc loc;
	CMTokenType type;
	CMStringView value;
} CMToken;


typedef struct CMTokenList
{
	CMToken *tokens;
	size_t len;
	size_t cap;
} CMTokenList;


CMToken CM_TOKEN_NULL = {0};



const char *cm_readable_token_type (CMTokenType type)
{
	return CM_TOKEN_TYPES_READABLE[type];
}


const char *cm_token_type_symbol (CMTokenType type)
{
	return CM_TOKEN_TYPE_SYMBOLS[type];
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


CMTokenList cm_tokenlist ()
{
	CMTokenList list;

	list.tokens = malloc(CM_TOKEN_LIST_BLOCK_SIZE);
	list.len = 0;
	list.cap = CM_TOKEN_LIST_BLOCK_SIZE;

	return list;
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

	return list.tokens[i];
}


CMToken cm_tokenlist_last (CMTokenList list)
{
	if (list.len < 1) {
		assert(false && "Attempted to access empty list with cm_tokenlist_last");
	}

	return cm_tokenlist_get(list, list.len - 1);
}


bool cm_tokenlist_like (CMTokenList list, CMTokenType types[], size_t types_len)
{
	if (list.len < types_len) {
		return false;
	}

	for (size_t i = 0; i < types_len; i += 1) {
		if (cm_tokenlist_get(list, i).type != types[i]) {
			return false;
		}
	}

	return true;
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
	if (list->len + 1 > list->cap) {
		cm_tokenlist_realloc(list);
	}

	list->tokens[list->len] = token;
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
}


bool cm_is_alnum (char c)
{
	return (bool) isalnum(c);
}


void cm_syntax_error (CMToken token, const char *message)
{
	fprintf(stderr, "FAILURE %s:%zu:%zu: %s", token.loc.filename, token.loc.row, token.loc.col, message);
	exit(CM_ERROR_EXIT_SYNTAX);
}


CMTokenList cm_tokenize (const char *filename, CMStringView sv)
{
	size_t row = 0;
	size_t col = 0;

	CMTokenList list = cm_tokenlist();
	bool in_quotes = false;

	assert(CM_TOKEN_TYPE_COUNT == 22);

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
			word.value = cm_chop_left_while(&sv, cm_is_alnum);

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

#endif // CM_TOKENIZER_H