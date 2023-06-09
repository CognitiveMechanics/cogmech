
#ifndef CM_TOKENLIST_H
#define CM_TOKENLIST_H

#ifndef CM_TOKEN_LIST_BLOCK_SIZE
#define CM_TOKEN_LIST_BLOCK_SIZE 2048
#endif

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "file.h"
#include "util.h"
#include "stringview.h"
#include "token.h"


typedef struct CMTokenList
{
	CMToken *tokens;
	size_t len;
	size_t cap;
	size_t cur;
} CMTokenList;


CMTokenList cm_tokenlist (void);
CMTokenList cm_tokenlist_clone (CMTokenList list);
CMToken cm_tokenlist_get (CMTokenList list, size_t i);
CMToken cm_tokenlist_first (CMTokenList list);
CMToken cm_tokenlist_last (CMTokenList list);
size_t cm_tokenlist_len (CMTokenList list);

#define cm_tokenlist_like(list, types) _cm_tokenlist_like(list, types, ARRAY_LEN(types))
bool _cm_tokenlist_like (CMTokenList list, const CMTokenType types[], size_t types_len);

bool cm_tokenlist_first_like (CMTokenList list, CMTokenType type);
CMToken cm_tokenlist_shift (CMTokenList *list);
CMToken cm_tokenlist_expect (CMTokenList *list, CMTokenType type, const char *message);
void cm_tokenlist_skip_endl (CMTokenList *list);

bool cm_tokenlist_empty (CMTokenList list);
void cm_print_tokenlist (CMTokenList list);
void cm_tokenlist_realloc (CMTokenList *list);

void cm_tokenlist_append (CMTokenList *list, CMToken token);
void cm_tokenlist_clear (CMTokenList *list);
void cm_tokenlist_free (CMTokenList *list);


#endif // CM_TOKENLIST_H
