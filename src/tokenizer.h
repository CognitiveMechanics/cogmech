
#ifndef CM_TOKENIZER_H
#define CM_TOKENIZER_H

#define CM_ERROR_EXIT_SYNTAX 2

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "file.h"
#include "util.h"
#include "stringview.h"
#include "token.h"
#include "tokenlist.h"
#include "context.h"


void cm_syntax_error (CMToken token, const char *message);

CMTokenList cm_tokenize (const char *filename, CMStringView sv);
CMTokenList cm_tokenize_file (const char *filename);


#endif // CM_TOKENIZER_H
