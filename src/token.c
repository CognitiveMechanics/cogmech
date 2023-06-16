
#include "token.h"


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
	"CM_TOKEN_TYPE_D_COLON",
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
	NULL,
	NULL,
	"*",
	"#",
	"->",
	"=>",
	"+",
	"-",
	"::",
};


const char *cm_readable_token_type (CMTokenType type)
{
	return CM_TOKEN_TYPES_READABLE[type];
}


const char *cm_token_type_symbol (CMTokenType type)
{
	return CM_TOKEN_TYPE_SYMBOLS[type];
}


CMLoc cm_loc (const char * filename, size_t row, size_t col)
{
	assert(filename != NULL);

	CMLoc loc = {
		.filename = malloc(strlen(filename) + 1),
		.row = row,
		.col = col
	};

	strcpy(loc.filename, filename);

	return loc;
}


CMToken cm_token (const char * filename, size_t row, size_t col, CMTokenType type)
{
	return (CMToken) {
		.loc = cm_loc(filename, row, col),
		.type = type,
		.value = CM_SV_NULL,
		.macro = NULL
	};
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
