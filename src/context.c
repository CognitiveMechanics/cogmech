
#include "context.h"


CMContext cm_context (void)
{
	return (CMContext) {0};
}


CMContext cm_context_clone (const CMContext context)
{
	CMContext new_context = (CMContext) {
		.n_symbol_defs = context.n_symbol_defs,
		.n_op_defs = context.n_op_defs,
		.n_relation_defs = context.n_relation_defs,
	};

	for (size_t i = 0; i < context.n_symbol_defs; i++) {
		CMSymbolDef def = context.symbol_defs[i];

		new_context.symbol_defs[i] = (CMSymbolDef) {
			.name = def.name,
			.value = cm_node_clone(def.value),
		};
	}

	for (size_t i = 0; i < context.n_op_defs; i++) {
		CMOpDef def = context.op_defs[i];

		new_context.op_defs[i] = (CMOpDef) {
			.name = def.name,
			.arglist = cm_node_clone(def.arglist),
			.body = cm_node_clone(def.body),
		};
	}

	for (size_t i = 0; i < context.n_relation_defs; i++) {
		CMRelationDef def = context.relation_defs[i];

		new_context.relation_defs[i] = (CMRelationDef) {
			.bind = def.bind,
			.state = cm_node_clone(def.state),
			.body = cm_node_clone(def.body),
		};
	}

	return new_context;
}


bool cm_context_has_symbol (CMContext *context, CMStringView name)
{
	for (size_t i = 0; i < context->n_symbol_defs; i++) {
		CMSymbolDef def = context->symbol_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return true;
		}
	}

	return false;
}


CMNode *cm_context_get_symbol (CMContext *context, CMStringView name)
{
	assert(cm_context_has_symbol(context, name));

	for (size_t i = 0; i < context->n_symbol_defs; i++) {
		CMSymbolDef def = context->symbol_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return def.value;
		}
	}

	assert(false && "Tried getting undefined symbol");
}


void cm_context_def_symbol (CMContext *context, CMStringView name, CMNode *value)
{
	assert(context->n_symbol_defs < CM_MAX_SYMBOLS);
	assert(! cm_context_has_symbol(context, name));

	context->symbol_defs[context->n_symbol_defs] = (CMSymbolDef) {name, value};
	context->n_symbol_defs += 1;
}


void cm_context_redef_symbol (CMContext *context, CMStringView name, CMNode *value)
{
	for (size_t i = 0; i < context->n_symbol_defs; i++) {
		if (cm_sv_eq(context->symbol_defs[i].name, name)) {
			context->symbol_defs[i].value = value;
			return;
		}
	}

	assert(false && "Tried overwriting undefined symbol");
}


void cm_context_force_def_symbol (CMContext *context, CMStringView name, CMNode *value)
{
	if (cm_context_has_symbol(context, name)) {
		cm_context_redef_symbol(context, name, value);
	} else {
		cm_context_def_symbol(context, name, value);
	}
}


bool cm_context_has_op (CMContext *context, CMStringView name)
{
	for (size_t i = 0; i < context->n_op_defs; i++) {
		CMOpDef def = context->op_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return true;
		}
	}

	return false;
}


CMOpDef cm_context_get_op (CMContext *context, CMStringView name)
{
	assert(cm_context_has_op(context, name));

	for (size_t i = 0; i < context->n_op_defs; i++) {
		CMOpDef def = context->op_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return def;
		}
	}

	assert(false && "Tried getting undefined op");
}


void cm_context_def_op (CMContext *context, CMStringView name, CMNode *arglist, CMNode *body)
{
	assert(context->n_op_defs < CM_MAX_OPS);
	assert(! cm_context_has_op(context, name));

	context->op_defs[context->n_op_defs] = (CMOpDef) {name, arglist, body};
	context->n_op_defs += 1;
}


CMRelationDef *cm_context_get_matching_relation (CMContext *context, CMNode *query)
{
	for (size_t i = 0; i < context->n_relation_defs; i++) {
		if (cm_match(query, context->relation_defs[i].state)) {
			return &context->relation_defs[i];
		}
	}

	return NULL;
}


void cm_context_def_relation (CMContext *context, CMStringView bind, CMNode *state, CMNode *body)
{
	assert(context->n_relation_defs < CM_MAX_RELATIONS);

	context->relation_defs[context->n_relation_defs] = (CMRelationDef) {bind, state, body};
	context->n_relation_defs += 1;
}




bool cm_static_context_has_macro (CMContext *context, CMStringView name)
{
	for (size_t i = 0; i < context->n_macro_defs; i++) {
		CMMacroDef def = context->macro_defs[i];

		if (cm_sv_eq(def.token.value, name)) {
			return true;
		}
	}

	return false;
}


CMMacroDef cm_static_context_get_macro (CMContext *context, CMStringView name)
{
	for (size_t i = 0; i < context->n_macro_defs; i++) {
		CMMacroDef def = context->macro_defs[i];

		if (cm_sv_eq(def.token.value, name)) {
			return def;
		}
	}

	assert(false && "Macro not defined");
}


void cm_static_context_def_macro(CMContext *context, CMToken word, CMTokenList body)
{
	assert(word.type == CM_TOKEN_TYPE_WORD);
	assert(! cm_static_context_has_macro(context, word.value) && "Attempt to redefine macro");
	assert(context->n_macro_defs < CM_MACRO_DEFS - 1);

	context->macro_defs[context->n_macro_defs] = (CMMacroDef) {
		.token = word,
		.body = body
	};

	context->n_macro_defs += 1;
}
