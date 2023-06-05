
#ifndef CM_INTERPRETER_H
#define CM_INTERPRETER_H

#include <stdbool.h>
#include "parser.h"

#define CM_MAX_SYMBOLS 2048
#define CM_MAX_RELATIONS 2048
#define CM_MAX_OPS 2048


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


typedef struct CMContext {
	CMSymbolDef symbol_defs[CM_MAX_SYMBOLS];
	size_t n_symbol_defs;

	CMOpDef op_defs[CM_MAX_OPS];
	size_t n_op_defs;

	CMRelationDef relation_defs[CM_MAX_RELATIONS];
	size_t n_relation_defs;
} CMContext;


bool cm_match (CMNode *match, CMNode *against);

CMContext cm_context (void);
CMContext cm_context_clone (const CMContext context);

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

void cm_print_entity (CMNode *node);
CMNode *cm_create_key_value (CMNode *key, CMNode *value);

CMNode *cm_interpret_entity (CMContext *context, CMNode *node);
CMNode *cm_interpret_compose (CMContext *context, CMNode *node);
CMNode *cm_interpret_extract (CMContext *context, CMNode *node);
CMNode *cm_interpret_transclude (CMContext *context, CMNode *node);
CMNode *cm_interpret_match (CMContext *context, CMNode *node);
CMNode *cm_interpret_dot (CMContext *context, CMNode *node);
CMNode *cm_interpret_eval (CMContext *context, CMNode *node);
CMNode *cm_interpret_op_invoke (CMContext *context, CMNode *node);
CMNode *cm_interpret_entity (CMContext *context, CMNode *node);

void cm_interpret_symbol_def (CMContext *context, CMNode *node);
void cm_interpret_op_def (CMContext *context, CMNode *node);
void cm_interpret_relation_def (CMContext *context, CMNode *node);
void cm_interpret_print (CMContext *context, CMNode *node);

void cm_interpret (CMContext *context, CMNode *ast);


#endif // CM_INTERPRETER_H
