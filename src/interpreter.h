
#ifndef CM_INTERPRETER_H
#define CM_INTERPRETER_H

#include <stdbool.h>
#include "parser.h"

#define CM_MAX_SYMBOLS 2048


typedef struct CMSymbolDef {
	CMStringView name;
	CMNode *value;
} CMSymbolDef;


typedef struct CMContext {
	CMSymbolDef symbol_defs[CM_MAX_SYMBOLS];
	size_t n_symbol_defs;
} CMContext;


CMContext cm_context (void)
{
	return (CMContext) {0};
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
	for (size_t i = 0; i < context->n_symbol_defs; i++) {
		CMSymbolDef def = context->symbol_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return def.value;
		}
	}

	// TODO: should be syntax error
	assert(false && "invalid symbol definition lookup");
}


void cm_context_def_symbol (CMContext *context, CMStringView name, CMNode *value)
{
	assert(context->n_symbol_defs < CM_MAX_SYMBOLS);
	assert(! cm_context_has_symbol(context, name));

	context->symbol_defs[context->n_symbol_defs] = (CMSymbolDef) {name, value};
	context->n_symbol_defs += 1;
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
	switch (node->type) {
		case CM_NODE_TYPE_NULL: {
			printf(
				"%*snull\n",
				indent_level * num_spaces,
				""
			);

			break;
		}
		case CM_NODE_TYPE_TRUE: {
			printf(
				"%*strue\n",
				indent_level * num_spaces,
				""
			);

			break;
		}

		case CM_NODE_TYPE_PROXY: {
			printf(
				"%*s[]\n",
				indent_level * num_spaces,
				""
			);

			break;
		}

		case CM_NODE_TYPE_DOT_PROXY: {
			printf(
				"%*s[.]\n",
				indent_level * num_spaces,
				""
			);

			break;
		}

		case CM_NODE_TYPE_KEY: {
			printf(
				"%*skey\n",
				indent_level * num_spaces,
				""
			);

			break;
		}

		case CM_NODE_TYPE_LITERAL: {
			printf(
				"%*s\"%.*s\"\n",
				indent_level * num_spaces,
				"",
				(int) node->value.len,
				node->value.data
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

				if (with_comma) {
					printf("%*s>,\n", indent_level * num_spaces, "");
				} else {
					printf("%*s>\n", indent_level * num_spaces, "");
				}
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

					} else if (child->type == CM_NODE_TYPE_NULL) {
						printf("null");

					} else if (child->type == CM_NODE_TYPE_TRUE) {
						printf("true");

					} else if (child->type == CM_NODE_TYPE_PROXY) {
						printf("[]");

					} else if (child->type == CM_NODE_TYPE_DOT_PROXY) {
						printf("[.]");

					} else if (child->type == CM_NODE_TYPE_KEY) {
						printf("key");

					} else {
						assert(false && "Invalid node to print");
					}

					if (i < node->n_children - 1) {
						printf(", ");
					}
				}

				if (with_comma) {
					printf(">,\n");
				} else {
					printf(">\n");
				}
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


bool cm_match (CMNode *match, CMNode *against)
{
	assert(match);
	assert(against);

	bool is_equal = cm_node_eq(match, against);
	bool against_is_proxy = (against->type == CM_NODE_TYPE_PROXY);
	bool proxy_against_dot_proxy = (match->type == CM_NODE_TYPE_PROXY && against->type == CM_NODE_TYPE_DOT_PROXY);

	if (is_equal || against_is_proxy || proxy_against_dot_proxy) {
		return true;
	}

	if (match->type != against->type) {
		return false;
	}

	if (cm_node_type_has_value(against->type)) {
		if (! cm_sv_eq(match->value, against->value)) {
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


CMNode *cm_interpret_entity (CMContext *context, CMNode *node);


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


CMNode *cm_interpret_entity (CMContext *context, CMNode *node)
{
	switch (node->type) {
		case CM_NODE_TYPE_LITERAL: {
			return cm_node_literal(node->value);
		}

		case CM_NODE_TYPE_SYMBOL: {
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

		case CM_NODE_TYPE_DOT: {
			return cm_interpret_dot(context, node);
		}

		case CM_NODE_TYPE_PROXY:
		case CM_NODE_TYPE_DOT_PROXY:
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
		// TODO: update to syntax error
		assert(false && "Symbol redefined");
	}

	CMNode *interpreted = cm_interpret_entity(context, value);
	cm_context_def_symbol(context, symbol->value, interpreted);
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
	assert(CM_NODE_TYPE_COUNT == 15);

	assert(ast->type == CM_NODE_TYPE_ROOT);

	for (size_t i = 0; i < ast->n_children; i++) {
		CMNode *stmt = ast->children[i];

		switch (stmt->type) {
			case CM_NODE_TYPE_SYMBOL_DEF: {
				cm_interpret_symbol_def(context, stmt);
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


#endif // CM_INTERPRETER_H
