
#ifndef CM_TOKENIZER_H
#define CM_TOKENIZER_H

#ifndef CM_TOKEN_LIST_BLOCK_SIZE
#define CM_TOKEN_LIST_BLOCK_SIZE 2048
#endif


#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "stringview.h"


typedef struct CMLoc
{
	const char * filename;
	size_t row;
	size_t col;
} CMLoc;


typedef enum CMTokenType
{
	CM_TOKEN_TYPE_WORD = 0,
	CM_TOKEN_TYPE_LT,
	CM_TOKEN_TYPE_GT,
	CM_TOKEN_TYPE_QUOTED,
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


CMTokenList cm_tokenlist ()
{
	CMTokenList list;

	list.tokens = malloc(CM_TOKEN_LIST_BLOCK_SIZE);
	list.len = 0;
	list.cap = CM_TOKEN_LIST_BLOCK_SIZE;

	return list;
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


bool cm_is_alpha (char c)
{
	return (bool) isalpha(c);
}


bool cm_is_alnum (char c)
{
	return (bool) isalnum(c);
}


CMTokenList cm_tokenize (const char *filename, CMStringView sv)
{
	size_t row = 0;
	size_t col = 0;

	CMTokenList list = cm_tokenlist();
	bool in_quotes = false;

	assert(CM_TOKEN_TYPE_COUNT == 20);

//	while (! cm_sv_empty(sv)) {
//		size_t trimmed = cm_trim_left_ws(&sv);
//		col += trimmed;
//
//		if (cm_starts_with(sv, cm_sv("<"))) {
//			row += 1;
//			cm_trim_left(&sv, "<");
//
//			cm_tokenlist_append(
//				&list,
//				cm_token(filename, row, col, CM_TOKEN_TYPE_GT)
//			);
//
//		} else if (cm_starts_with(sv, cm_sv(">"))) {
//			row += 1;
//			cm_trim_left(&sv, ">");
//
//			cm_tokenlist_append(
//				&list,
//				cm_token(filename, row, col, CM_TOKEN_TYPE_LT)
//			);
//
//		} else if (cm_starts_with(sv, cm_sv(":="))) {
//			row += 2;
//			cm_trim_left(&sv, ":=");
//
//			cm_tokenlist_append(
//				&list,
//				cm_token(filename, row, col, CM_TOKEN_TYPE_COLON_EQ)
//			);
//
//		} else if (cm_starts_with(sv, cm_sv("\""))) {
//			CMToken quoted = cm_token(filename, row, col, CM_TOKEN_TYPE_QUOTED);
//			size_t curr = 1;
//
//			while (curr < sv.len) {
//				if (sv.data[curr] == '"') {
//				}
//
//				curr += 1;
//			}
//
//			// TODO: improve tokenizer error handling
//			assert(0 && "Unterminated quote");
//
//		}
//	}

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