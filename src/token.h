
#ifndef CM_TOKEN_H
#define CM_TOKEN_H

#ifndef CM_TOKEN_LIST_BLOCK_SIZE
#define CM_TOKEN_LIST_BLOCK_SIZE 2048
#endif

#define CM_ERROR_EXIT_SYNTAX 2
#define CM_MACRO_DEFS 2048


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
	CM_TOKEN_TYPE_INCLUDE,
	CM_TOKEN_TYPE_D_COLON,
	CM_TOKEN_TYPE_COUNT,
} CMTokenType;


typedef struct CMToken CMToken;

typedef struct CMToken
{
	CMLoc loc;
	CMTokenType type;
	CMStringView value;
	CMToken *macro;
} CMToken;


static CMToken CM_TOKEN_NULL = {0};


const char *cm_readable_token_type (CMTokenType type);
const char *cm_token_type_symbol (CMTokenType type);

CMToken cm_token (const char * filename, size_t row, size_t col, CMTokenType type);
bool cm_token_eq (CMToken token1, CMToken token2);
void cm_print_token (CMToken token);
void cm_print_token_indented (CMToken token, int indent_level, int num_spaces);


#endif // CM_TOKEN_H
