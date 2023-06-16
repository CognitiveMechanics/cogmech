
#ifndef CM_CONTEXT_H
#define CM_CONTEXT_H

#define CM_MAX_SYMBOLS 2048
#define CM_MAX_RELATIONS 2048
#define CM_MAX_OPS 2048
#define CM_MAX_MACROS 2048

#include <stdbool.h>

#include "stringview.h"
#include "node.h"
#include "token.h"
#include "tokenlist.h"


typedef struct CMSymbolDef {
	CMStringView name;
	CMNode *value;
} CMSymbolDef;


typedef struct CMOpDef {
	CMStringView name;
	CMNode *arglist;
	CMNode *body;
} CMOpDef;


typedef struct CMRelationDef {
	CMStringView bind;
	CMNode *state;
	CMNode *body;
} CMRelationDef;


typedef struct CMMacroDef
{
	CMToken token;
	CMTokenList body;
} CMMacroDef;


typedef struct CMContext {
	CMSymbolDef symbol_defs[CM_MAX_SYMBOLS];
	size_t n_symbol_defs;

	CMOpDef op_defs[CM_MAX_OPS];
	size_t n_op_defs;

	CMRelationDef relation_defs[CM_MAX_RELATIONS];
	size_t n_relation_defs;

	CMMacroDef macro_defs[CM_MAX_MACROS];
	size_t n_macro_defs;

	size_t trace;
} CMContext;


CMContext cm_context (void);
CMContext cm_context_clone (const CMContext *context);

bool cm_context_has_symbol (CMContext *context, CMStringView name);
CMNode *cm_context_get_symbol (CMContext *context, CMStringView name);
void cm_context_def_symbol (CMContext *context, CMStringView name, CMNode *value);
void cm_context_redef_symbol (CMContext *context, CMStringView name, CMNode *value);
void cm_context_force_def_symbol (CMContext *context, CMStringView name, CMNode *value);

bool cm_context_has_op (CMContext *context, CMStringView name);
CMOpDef cm_context_get_op (CMContext *context, CMStringView name);
void cm_context_def_op (CMContext *context, CMStringView name, CMNode *arglist, CMNode *body);

void cm_context_def_relation (CMContext *context, CMStringView bind, CMNode *state, CMNode *body);
CMRelationDef *cm_context_get_matching_relation (CMContext *context, CMNode *query);

bool cm_static_context_has_macro(CMContext *context, CMStringView name);
CMMacroDef cm_static_context_get_macro(CMContext *context, CMStringView name);
void cm_static_context_def_macro(CMContext *context, CMToken word, CMTokenList body);


#endif // CM_CONTEXT_H
