
#ifndef CM_TOKENIZER_H
#define CM_TOKENIZER_H

#ifndef CM_TOKEN_LIST_BLOCK_SIZE
#define CM_TOKEN_LIST_BLOCK_SIZE 2048
#endif

#define CM_ERROR_EXIT_SYNTAX 2


#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "file.h"
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
	CM_TOKEN_TYPE_INT,
	CM_TOKEN_TYPE_QUOTED,
	CM_TOKEN_TYPE_ENDL,
	CM_TOKEN_TYPE_LT,
	CM_TOKEN_TYPE_GT,
	CM_TOKEN_TYPE_COMMA,
	CM_TOKEN_TYPE_COLON_EQ,
	CM_TOKEN_TYPE_COLON,
	CM_TOKEN_TYPE_PROXY,
	CM_TOKEN_TYPE_DOT_PROXY,
	CM_TOKEN_TYPE_SQ_BRACKET_IN,
	CM_TOKEN_TYPE_SQ_BRACKET_OUT,
	CM_TOKEN_TYPE_PAREN_IN,
	CM_TOKEN_TYPE_PAREN_OUT,
	CM_TOKEN_TYPE_PERCENT,
	CM_TOKEN_TYPE_TRUE,
	CM_TOKEN_TYPE_NULL,
	CM_TOKEN_TYPE_DOT,
	CM_TOKEN_TYPE_HASH,
	CM_TOKEN_TYPE_S_ARROW,
	CM_TOKEN_TYPE_D_ARROW,
	CM_TOKEN_TYPE_PLUS,
	CM_TOKEN_TYPE_MINUS,
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
	size_t cur;
} CMTokenList;


static CMToken CM_TOKEN_NULL = {0};


const char *cm_readable_token_type (CMTokenType type);
const char *cm_token_type_symbol (CMTokenType type);
void cm_syntax_error (CMToken token, const char *message);
CMToken cm_token (const char * filename, size_t row, size_t col, CMTokenType type);
void cm_print_token (CMToken token);
CMTokenList cm_tokenlist (void);
CMTokenList cm_tokenlist_clone (CMTokenList list);
bool cm_token_eq (CMToken token1, CMToken token2);
CMToken cm_tokenlist_get (CMTokenList list, size_t i);
CMToken cm_tokenlist_first (CMTokenList list);
CMToken cm_tokenlist_last (CMTokenList list);

#define cm_tokenlist_like(list, types) _cm_tokenlist_like(list, types, ARRAY_LEN(types))
bool _cm_tokenlist_like (CMTokenList list, const CMTokenType types[], size_t types_len);

bool cm_tokenlist_first_like (CMTokenList list, CMTokenType type);
CMToken cm_tokenlist_shift (CMTokenList *list);
CMToken cm_tokenlist_expect (CMTokenList *list, CMTokenType type);
void cm_tokenlist_skip_endl (CMTokenList *list);

bool cm_tokenlist_empty (CMTokenList list);
void cm_print_tokenlist (CMTokenList list);
void cm_tokenlist_realloc (CMTokenList *list);

void cm_tokenlist_append (CMTokenList *list, CMToken token);
void cm_tokenlist_clear (CMTokenList *list);
void cm_tokenlist_free (CMTokenList *list);
bool cm_is_word (char c);

CMTokenList cm_tokenize (const char *filename, CMStringView sv);
CMTokenList cm_tokenize_file (const char *filename);

#endif // CM_TOKENIZER_H
