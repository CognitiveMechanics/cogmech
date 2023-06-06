

#include <stdbool.h>
#include <stdlib.h>

#include "stringview.h"
#include "parser.h"
#include "interpreter.h"


bool cm_match (CMNode *match, CMNode *against)
{
	assert(match);
	assert(against);

	bool is_equal = cm_node_eq(match, against);

	bool against_is_proxy        = (against->type == CM_NODE_TYPE_PROXY);
	bool proxy_against_dot_proxy = (match->type == CM_NODE_TYPE_PROXY && against->type == CM_NODE_TYPE_DOT_PROXY);

	bool int_vs_int       = (match->type == CM_NODE_TYPE_INT && against->type == CM_NODE_TYPE_INT);
	bool int_vs_exact_int = (match->type == CM_NODE_TYPE_INT && against->type == CM_NODE_TYPE_INT_EXACT);

	if (is_equal || against_is_proxy || proxy_against_dot_proxy) {
		return true;
	}

	if (int_vs_int) {
		cm_int match_value = cm_node_int_value(match);
		cm_int against_value = cm_node_int_value(against);

		return match_value >= against_value;
	}

	if (int_vs_exact_int) {
		cm_int match_value = cm_node_int_value(match);
		cm_int against_value = cm_node_int_value(against);

		return match_value == against_value;
	}

	if (match->type != against->type) {
		return false;
	}

	if (cm_node_type_has_value(against->type)) {
		if (! cm_sv_eq(match->value, against->value)) {
			return false;
		}
	}

	if (cm_node_type_has_int_value(against->type)) {
		if (cm_node_int_value(match) != cm_node_int_value(against)) {
			return false;
		}
	}

	for (size_t i = 0; i < against->n_children; i++) {
		bool matched = false;

		for (size_t j = 0; j < match->n_children; j++) {
			bool result = cm_match(match->children[j], against->children[i]);

			if (result) {
				matched = true;
				break;
			}
		}

		if (! matched) {
			return false;
		}
	}

	return true;
}


CMContext cm_context (void)
{
	return (CMContext) {0};
}


CMContext cm_context_clone (const CMContext context)
{
	CMContext new_context = (CMContext) {
		.n_symbol_defs = context.n_symbol_defs,
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


bool _cm_print_entity_has_composition (CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_COMPOSE);

	for (size_t i = 0; i < node->n_children; i++) {
		if (node->children[i]->type == CM_NODE_TYPE_COMPOSE) {
			return true;
		}
	}

	return false;
}


bool _cm_print_entity_should_indent (CMNode *node)
{
	bool is_composition = (node->type == CM_NODE_TYPE_COMPOSE);
	bool has_composition = _cm_print_entity_has_composition(node);
	bool has_max_nodes = (node->n_children > 3);

	return is_composition && (has_composition || has_max_nodes);
}


void _cm_print_entity (CMNode *node, int indent_level, int num_spaces, bool with_comma)
{
	char *comma;

	if (with_comma) {
		comma = ",";
	} else {
		comma = "";
	}

	switch (node->type) {
		case CM_NODE_TYPE_NULL: {
			printf(
				"%*snull%s\n",
				indent_level * num_spaces,
				"",
				comma
			);

			break;
		}

		case CM_NODE_TYPE_TRUE: {
			printf(
				"%*strue%s\n",
				indent_level * num_spaces,
				"",
				comma
			);

			break;
		}

		case CM_NODE_TYPE_PROXY: {
			printf(
				"%*s[]%s\n",
				indent_level * num_spaces,
				"",
				comma
			);

			break;
		}

		case CM_NODE_TYPE_DOT_PROXY: {
			printf(
				"%*s[.]%s\n",
				indent_level * num_spaces,
				"",
				comma
			);

			break;
		}

		case CM_NODE_TYPE_KEY: {
			printf(
				"%*skey%s\n",
				indent_level * num_spaces,
				"",
				comma
			);

			break;
		}

		case CM_NODE_TYPE_LITERAL: {
			printf(
				"%*s\"%.*s\"%s\n",
				indent_level * num_spaces,
				"",
				(int) node->value.len,
				node->value.data,
				comma
			);

			break;
		}

		case CM_NODE_TYPE_INT: {
			printf(
				"%*s%.*s%s\n",
				indent_level * num_spaces,
				"",
				(int) node->value.len,
				node->value.data,
				comma
			);

			break;
		}

		case CM_NODE_TYPE_INT_EXACT: {
			printf(
				"%*s*%.*s%s\n",
				indent_level * num_spaces,
				"",
				(int) node->value.len,
				node->value.data,
				comma
			);

			break;
		}

		case CM_NODE_TYPE_COMPOSE: {
			if (_cm_print_entity_should_indent(node)) {
				printf("%*s<\n", indent_level * num_spaces, "");

				for (size_t i = 0; i < node->n_children; i++) {
					_cm_print_entity(
						node->children[i],
						indent_level + 1,
						num_spaces,
						i < node->n_children - 1
					);
				}

				printf("%*s>%s\n", indent_level * num_spaces, "", comma);
			} else {
				printf("%*s<", indent_level * num_spaces, "");

				for (size_t i = 0; i < node->n_children; i++) {
					CMNode *child = node->children[i];

					if (child->type == CM_NODE_TYPE_LITERAL) {
						printf(
							"\"%.*s\"",
							(int) child->value.len,
							child->value.data
						);

					} else if (child->type == CM_NODE_TYPE_INT) {
						printf(
							"%.*s",
							(int) child->value.len,
							child->value.data
						);

					} else if (child->type == CM_NODE_TYPE_INT_EXACT) {
						printf(
							"*%.*s",
							(int) child->value.len,
							child->value.data
						);

					} else if (child->type == CM_NODE_TYPE_NULL) {
						printf("null");

					} else if (child->type == CM_NODE_TYPE_TRUE) {
						printf("true");

					} else if (child->type == CM_NODE_TYPE_PROXY) {
						printf("[]");

					} else if (child->type == CM_NODE_TYPE_DOT_PROXY) {
						printf("[*]");

					} else if (child->type == CM_NODE_TYPE_KEY) {
						printf("key");

					} else {
						assert(false && "Invalid node to print");
					}

					if (i < node->n_children - 1) {
						printf(", ");
					}
				}

				printf(">%s\n", comma);
			}

			break;
		}

		default: {
			assert(false && "Nonprintable type");
		}
	}
}


void cm_print_entity (CMNode *node)
{
	_cm_print_entity(node, 0, 2, false);
}


CMNode *cm_create_key_value (CMNode *key, CMNode *value)
{
	assert(key);
	assert(value);

	CMNode *pair = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(pair, key);
	cm_node_append_child(pair, value);

	return pair;
}


CMNode *cm_interpret_compose (CMContext *context, CMNode *node)
{
	CMNode *def_value = cm_node(CM_NODE_TYPE_COMPOSE);

	for (size_t i = 0; i < node->n_children; i++) {
		cm_node_append_child(
			def_value,
			cm_interpret_entity(context, node->children[i])
		);
	}

	return def_value;
}


CMNode *cm_interpret_extract (CMContext *context, CMNode *node)
{
	assert(node->n_children == 2);
	assert(node->type == CM_NODE_TYPE_EXTRACT);

	CMNode *entity = cm_interpret_entity(context, node->children[0]);
	CMNode *key = cm_interpret_entity(context, node->children[1]);

	assert(entity->type == CM_NODE_TYPE_COMPOSE);

	for (size_t i = 0; i < entity->n_children; i++) {
		CMNode *child = entity->children[i];

		if (child->type == CM_NODE_TYPE_COMPOSE && child->n_children == 2) {
			if (cm_node_eq(child->children[0], key)) {
				return cm_interpret_entity(context, child->children[1]);

			} else if (cm_node_eq(child->children[1], key)) {
				return cm_interpret_entity(context, child->children[0]);
			}
		}
	}

	return cm_node_null();
}


CMNode *cm_interpret_transclude (CMContext *context, CMNode *node)
{
	assert(node->n_children == 3);
	assert(node->type == CM_NODE_TYPE_TRANSCLUDE);

	CMNode *entity = cm_interpret_entity(context, node->children[0]);
	CMNode *key = cm_interpret_entity(context, node->children[1]);
	CMNode *new_value = cm_interpret_entity(context, node->children[2]);

	assert(entity->type == CM_NODE_TYPE_COMPOSE);
	CMNode *new_entity = cm_node(CM_NODE_TYPE_COMPOSE);

	bool found = false;

	for (size_t i = 0; i < entity->n_children; i++) {
		CMNode *child = entity->children[i];

		if (child->type == CM_NODE_TYPE_COMPOSE && child->n_children == 2) {
			if (cm_node_eq(child->children[0], key) || cm_node_eq(child->children[1], key)) {
				found = true;
				cm_node_append_child(new_entity, cm_create_key_value(key, new_value));
			} else {
				cm_node_append_child(new_entity, child);
			}
		}
	}

	if (! found) {
		cm_node_append_child(new_entity, cm_create_key_value(key, new_value));
	}

	return new_entity;
}


CMNode *cm_interpret_match (CMContext *context, CMNode *node)
{
	assert(node->n_children == 2);
	assert(node->type == CM_NODE_TYPE_MATCH);

	CMNode *match = cm_interpret_entity(context, node->children[0]);
	CMNode *against = cm_interpret_entity(context, node->children[1]);

	bool result = cm_match(match, against);

	if (result) {
		return cm_node(CM_NODE_TYPE_TRUE);
	} else {
		return cm_node_null();
	}
}


CMNode *cm_interpret_increment (CMContext *context, CMNode *node)
{
	assert(node->n_children == 1);
	assert(node->type == CM_NODE_TYPE_INCREMENT);

	CMNode *entity = cm_interpret_entity(context, node->children[0]);

	assert(cm_node_type_has_int_value(entity->type));
	cm_int value = cm_node_int_value(entity);

	value += 1;

	CMNode *result = cm_node_int_from_int(value);
	result->type = entity->type;

	return result;
}


CMNode *cm_interpret_decrement (CMContext *context, CMNode *node)
{
	assert(node->n_children == 1);
	assert(node->type == CM_NODE_TYPE_DECREMENT);

	CMNode *entity = cm_interpret_entity(context, node->children[0]);

	assert(cm_node_type_has_int_value(entity->type));
	cm_int value = cm_node_int_value(entity);

	value -= 1;

	if (value < 0) {
		return cm_node_null();
	}

	CMNode *result = cm_node_int_from_int(value);
	result->type = entity->type;

	return result;
}


CMNode *_cm_interpret_dot (CMContext *context, CMNode *node)
{
	if (node->type == CM_NODE_TYPE_PROXY) {
		node->type = CM_NODE_TYPE_DOT_PROXY;
	} else {
		for (size_t i = 0; i < node->n_children; i++) {
			_cm_interpret_dot(context, node->children[i]);
		}
	}

	return node;
}


CMNode *cm_interpret_dot (CMContext *context, CMNode *node)
{
	assert(node->n_children == 1);
	assert(node->type == CM_NODE_TYPE_DOT);

	CMNode *entity = cm_interpret_entity(context, node->children[0]);
	CMNode *clone = cm_node_clone(entity);

	return _cm_interpret_dot(context, clone);
}


CMNode *cm_interpret_eval (CMContext *context, CMNode *node)
{
	assert(node->n_children == 1);
	assert(node->type == CM_NODE_TYPE_EVAL);

	CMNode *entity = cm_interpret_entity(context, node->children[0]);
	CMContext scope_context = cm_context_clone(*context);

	CMRelationDef *def = NULL;
	CMNode *new_entity = NULL;

	do {
		def = cm_context_get_matching_relation(context, entity);

		if (def != NULL) {
			cm_context_force_def_symbol(&scope_context, def->bind, entity);
			new_entity = cm_interpret_entity(&scope_context, def->body);
		}

		if (cm_node_eq(entity, new_entity)) {
			return entity;
		} else {
			entity = new_entity;
		}
	} while(def != NULL);

	return entity;
}


CMNode *cm_interpret_op_invoke (CMContext *context, CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_OP_INVOKE);

	if (! cm_context_has_op(context, node->value)) {
		cm_syntax_error(node->token, "Operation not defined");
	}

	CMOpDef def = cm_context_get_op(context, node->value);

	if (def.arglist->n_children != node->n_children) {
		cm_syntax_error(node->token, "Number of arguments do not match op definition");
	}

	CMContext scope = cm_context_clone(*context);

	for (size_t i = 0; i < def.arglist->n_children; i++) {
		CMNode *arg = def.arglist->children[i];
		assert(arg->type == CM_NODE_TYPE_SYMBOL);

		cm_context_force_def_symbol(
			&scope,
			arg->value,
			cm_interpret_entity(context, node->children[i])
		);
	}

	return cm_interpret_entity(&scope, def.body);
}


CMNode *cm_interpret_entity (CMContext *context, CMNode *node)
{
	switch (node->type) {
		case CM_NODE_TYPE_LITERAL: {
			return cm_node_literal(node->value);
		}

		case CM_NODE_TYPE_SYMBOL: {
			if (! cm_context_has_symbol(context, node->value)) {
				cm_syntax_error(node->token, "Undefined symbol");
			}

			return cm_context_get_symbol(context, node->value);
		}

		case CM_NODE_TYPE_COMPOSE: {
			return cm_interpret_compose(context, node);
		}

		case CM_NODE_TYPE_EXTRACT: {
			return cm_interpret_extract(context, node);
		}

		case CM_NODE_TYPE_TRANSCLUDE: {
			return cm_interpret_transclude(context, node);
		}

		case CM_NODE_TYPE_MATCH: {
			return cm_interpret_match(context, node);
		}

		case CM_NODE_TYPE_EVAL: {
			return cm_interpret_eval(context, node);
		}

		case CM_NODE_TYPE_OP_INVOKE: {
			return cm_interpret_op_invoke(context, node);
		}

		case CM_NODE_TYPE_INCREMENT: {
			return cm_interpret_increment(context, node);
		}

		case CM_NODE_TYPE_DECREMENT: {
			return cm_interpret_decrement(context, node);
		}

		case CM_NODE_TYPE_DOT: {
			return cm_interpret_dot(context, node);
		}

		case CM_NODE_TYPE_PROXY:
		case CM_NODE_TYPE_DOT_PROXY:
		case CM_NODE_TYPE_INT:
		case CM_NODE_TYPE_INT_EXACT:
		case CM_NODE_TYPE_TRUE:
		case CM_NODE_TYPE_NULL:
		case CM_NODE_TYPE_KEY: {
			return node;
		}

		default: {
			assert(false && "Invalid entity type");
		}
	}
}


void cm_interpret_symbol_def (CMContext *context, CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_SYMBOL_DEF);
	assert(node->n_children == 2);
	assert(node->children[0] != NULL);
	assert(node->children[1] != NULL);

	CMNode *symbol = node->children[0];
	CMNode *value = node->children[1];

	assert(symbol->type == CM_NODE_TYPE_SYMBOL);

	if (cm_context_has_symbol(context, symbol->value)) {
		cm_syntax_error(symbol->token, "Symbol redefined");
	}

	CMNode *interpreted = cm_interpret_entity(context, value);
	cm_context_def_symbol(context, symbol->value, interpreted);
}


void cm_interpret_op_def (CMContext *context, CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_OP_DEF);
	assert(node->n_children == 2);
	assert(node->children[0]->type == CM_NODE_TYPE_OP_ARGLIST);
	assert(node->children[1] != NULL);

	CMNode *arglist = node->children[0];
	CMNode *body = node->children[1];

	assert(arglist->type == CM_NODE_TYPE_OP_ARGLIST);

	if (cm_context_has_op(context, node->value)) {
		cm_syntax_error(node->token, "Op redefined");
	}

	cm_context_def_op(context, node->value, arglist, body);
}


void cm_interpret_relation_def (CMContext *context, CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_RELATION_DEF);
	assert(node->n_children == 3);
	assert(node->children[0] != NULL);
	assert(node->children[2] != NULL);

	CMNode *symbol = node->children[0];
	CMNode *format = node->children[1];
	CMNode *body = node->children[2];

	assert(symbol->type == CM_NODE_TYPE_SYMBOL && "Relation state muct be a symbol");
	CMStringView alias_name = symbol->value;;

	bool format_is_null = cm_node_eq(format, cm_node_null());

	if (format_is_null) {
		format = cm_interpret_entity(context, symbol);
	} else {
		format = cm_interpret_entity(context, format);
	}

	cm_context_def_relation(
		context,
		alias_name,
		cm_interpret_entity(context, format),
		body
	);
}


void cm_interpret_print (CMContext *context, CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_PRINT);
	assert(node->n_children == 1);
	assert(node->children[0] != NULL);

	CMNode *interpreted = cm_interpret_entity(context, node->children[0]);
	cm_print_entity(interpreted);
}


void cm_interpret (CMContext *context, CMNode *ast)
{
	assert(CM_NODE_TYPE_COUNT == 24);

	assert(ast->type == CM_NODE_TYPE_ROOT);

	for (size_t i = 0; i < ast->n_children; i++) {
		CMNode *stmt = ast->children[i];

		switch (stmt->type) {
			case CM_NODE_TYPE_SYMBOL_DEF: {
				cm_interpret_symbol_def(context, stmt);
				break;
			}

			case CM_NODE_TYPE_OP_DEF: {
				cm_interpret_op_def(context, stmt);
				break;
			}

			case CM_NODE_TYPE_RELATION_DEF: {
				cm_interpret_relation_def(context, stmt);
				break;
			}

			case CM_NODE_TYPE_PRINT: {
				cm_interpret_print(context, stmt);
				break;
			}

			default: {
				assert(false && "Invalid statement type");
			}
		}
	}
}
