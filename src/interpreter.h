
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


bool cm_has_symbol_def (CMContext *context, CMStringView name)
{
	for (size_t i = 0; i < context->n_symbol_defs; i++) {
		CMSymbolDef def = context->symbol_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return true;
		}
	}

	return false;
}


CMNode *cm_lookup_symbol_def (CMContext *context, CMStringView name)
{
	for (size_t i = 0; i < context->n_symbol_defs; i++) {
		CMSymbolDef def = context->symbol_defs[i];

		if (cm_sv_eq(def.name, name)) {
			return def.value;
		}
	}

	assert(false && "invalid symbol definition lookup");
}


void cm_add_symbol_def (CMContext *context, CMStringView name, CMNode *value)
{
	assert(context->n_symbol_defs < CM_MAX_SYMBOLS);
	assert(! cm_has_symbol_def(context, name));

	context->symbol_defs[context->n_symbol_defs] = (CMSymbolDef) {name, value};
	context->n_symbol_defs += 1;
}


CMNode *cm_interpret_entity (CMContext *context, CMNode *node)
{
	switch (node->type) {
		case CM_NODE_TYPE_LITERAL: {
			return cm_node_literal(node->value);
		}

		case CM_NODE_TYPE_SYMBOL: {
			return cm_lookup_symbol_def(context, node->value);
		}

		case CM_NODE_TYPE_COMPOSITION: {
			CMNode *def_value = cm_node(CM_NODE_TYPE_COMPOSITION);

			for (size_t i = 0; i < node->n_children; i++) {
				cm_node_append_child(
					def_value,
					cm_interpret_entity(context, node->children[i])
				);
			}

			return def_value;
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

	if (cm_has_symbol_def(context, symbol->value)) {
		// TODO: update to syntax error
		assert(false && "Symbol redefined");
	}

	CMNode *interpreted = cm_interpret_entity(context, value);
	cm_add_symbol_def(context, symbol->value, interpreted);
}


bool _cm_print_entity_has_composition (CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_COMPOSITION);

	for (size_t i = 0; i < node->n_children; i++) {
		if (node->children[i]->type == CM_NODE_TYPE_COMPOSITION) {
			return true;
		}
	}

	return false;
}


bool _cm_print_entity_should_indent (CMNode *node)
{
	bool is_composition = (node->type == CM_NODE_TYPE_COMPOSITION);
	bool has_composition = _cm_print_entity_has_composition(node);
	bool has_max_nodes = (node->n_children > 3);

	return is_composition && (has_composition || has_max_nodes);
}


void _cm_print_entity (CMNode *node, int indent_level, int num_spaces, bool with_comma)
{
	switch (node->type) {
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

		case CM_NODE_TYPE_COMPOSITION: {
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
					assert(node->children[i]->type == CM_NODE_TYPE_LITERAL);

					printf(
						"\"%.*s\"",
						(int) node->children[i]->value.len,
						node->children[i]->value.data
					);

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
	_cm_print_entity(node, 0, 4, false);
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
	assert(ast->type == CM_NODE_TYPE_ROOT);

	for (int i = 0; i < ast->n_children; i++) {
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