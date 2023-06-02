
#ifndef CM_PARSER_H
#define CM_PARSER_H

#include <stdlib.h>
#include <stdio.h>

#include "tokenizer.h"

#define CM_NODE_CHILDREN_BLOCK_SIZE 16


typedef enum CMNodeType {
	CM_NODE_TYPE_ROOT = 0,
	CM_NODE_TYPE_SYMBOL_DEF,
	CM_NODE_TYPE_LITERAL,
	CM_NODE_TYPE_SYMBOL,
	CM_NODE_TYPE_COMPOSITION,
	CM_NODE_TYPE_PRINT,
	CM_NODE_TYPE_COUNT
} CMNodeType;

const char *CM_NODE_TYPES_READABLE[CM_NODE_TYPE_COUNT] = {
	"CM_NODE_TYPE_ROOT",
	"CM_NODE_TYPE_SYMBOL_DEF",
	"CM_NODE_TYPE_LITERAL",
	"CM_NODE_TYPE_SYMBOL",
	"CM_NODE_TYPE_COMPOSITION",
	"CM_NODE_TYPE_PRINT",
};


typedef struct CMNode {
	CMNodeType type;
	struct CMNode **children;
	size_t n_children;
	size_t cap;
	CMStringView value;
} CMNode;


const char *cm_readable_node_type (CMNodeType type)
{
	return CM_NODE_TYPES_READABLE[type];
}


void cm_node_alloc_children (CMNode *node)
{
	node->children = calloc(CM_NODE_CHILDREN_BLOCK_SIZE, sizeof(CMNode *));

	assert(node->children != NULL);
	assert(node->children[0] == NULL);

	node->cap = CM_NODE_CHILDREN_BLOCK_SIZE;
}


CMNode *cm_node (CMNodeType type)
{
	CMNode* node = malloc(sizeof(CMNode));

	node->type = type;
	node->n_children = 0;
	node->value = CM_SV_NULL;

	cm_node_alloc_children(node);

	return node;
}


void cm_node_realloc_children (CMNode *node)
{
	node->cap = node->cap + CM_NODE_CHILDREN_BLOCK_SIZE;
	node->children = realloc(node->children, node->cap);

	assert(node->children != NULL);
}


void cm_node_append_child (CMNode *node, CMNode *child)
{
	if (node->n_children + 1 > node->cap) {
		cm_node_realloc_children(node);
	}

	node->children[node->n_children] = child;
	node->n_children += 1;
}


CMNode *cm_node_symbol (CMStringView identifier)
{
	CMNode *symbol_node = cm_node(CM_NODE_TYPE_SYMBOL);
	symbol_node->value = identifier;

	return symbol_node;
}


CMNode *cm_node_literal (CMStringView name)
{
	CMNode *literal_node = cm_node(CM_NODE_TYPE_LITERAL);
	literal_node->value = name;

	return literal_node;
}


void cm_node_free (CMNode *node)
{
	assert(node->children != NULL);

	for (size_t i = 0; i < node->n_children; i++) {
		if (node->children[i]->children != NULL) {
			cm_node_free(node->children[i]);
		}
	}

	free(node->children);
	node->children = NULL;
	node->n_children = 0;
	node->cap = 0;

	free(node);
	node = NULL;
}


char *_cm_print_node (CMNode *node, int indent_level, int num_spaces)
{
	printf(
		"%*sNode {\n"
		"%*s  .type = %s\n"
		"%*s  .value = %.*s\n"
		"%*s  .children = {",
		indent_level * num_spaces,
		"",
		indent_level * num_spaces,
		"",
		cm_readable_node_type(node->type),
		indent_level * num_spaces,
		"",
		node->value.len,
		node->value.data,
		indent_level * num_spaces,
		""
	);

	if (node->n_children) {
		printf("\n");

		for (size_t i = 0; i < node->n_children; i++) {
			_cm_print_node(node->children[i], indent_level + 1, num_spaces);
		}

		printf(
			"%*s  }\n"
			"%*s}\n",
			indent_level * num_spaces,
			"",
			indent_level * num_spaces,
			""
		);
	} else {
		printf(
			"}\n"
			"%*s}\n",
			indent_level * num_spaces,
			""
		);
	}

}


void cm_print_node (CMNode *node)
{
	_cm_print_node(node, 0, 4);
}


CMNode *cm_parse_composition (CMTokenList *list);


CMNode *cm_parse_expr (CMTokenList *list)
{
	CMToken token = cm_tokenlist_first(*list);

	switch (token.type) {
		case CM_TOKEN_TYPE_WORD: {
			cm_tokenlist_shift(list);
			return cm_node_symbol(token.value);
		}

		case CM_TOKEN_TYPE_QUOTED: {
			cm_tokenlist_shift(list);
			return cm_node_literal(token.value);
		}

		case CM_TOKEN_TYPE_LT: {
			return cm_parse_composition(list);
		}

		default: {
			cm_syntax_error(token, "Expected expression\n");
		}
	}

	assert(false && "Unreachable");
}


CMNode *cm_parse_composition (CMTokenList *list)
{
	CMNode *node = cm_node(CM_NODE_TYPE_COMPOSITION);
	CMToken initial = cm_tokenlist_shift(list);
	bool terminated = false;

	do {
		cm_node_append_child(
			node,
			cm_parse_expr(list)
		);

		CMToken next_token = cm_tokenlist_first(*list);

		if (next_token.type == CM_TOKEN_TYPE_COMMA) {
			cm_tokenlist_shift(list);
			continue;
		} else if (next_token.type == CM_TOKEN_TYPE_GT) {
			terminated = true;
			cm_tokenlist_shift(list);
			break;
		}
	} while (! cm_tokenlist_empty(*list));

	if (! terminated) {
		cm_syntax_error(initial, "Unterminated composition\n");
	}

	return node;
}


CMNode *cm_parse_symbol_def (CMTokenList *list)
{
	CMNode *node = cm_node(CM_NODE_TYPE_SYMBOL_DEF);

	CMTokenType fmt_assignment[] = {
		CM_TOKEN_TYPE_WORD,
		CM_TOKEN_TYPE_COLON_EQ,
	};

	if (! cm_tokenlist_like(*list, fmt_assignment)) {
		assert(false && "Invalid token list for symbol definition");
	}

	CMToken symbol_token = cm_tokenlist_shift(list);
	CMNode *symbol = cm_node_symbol(symbol_token.value);
	cm_node_append_child(node, symbol);

	// discard :=
	cm_tokenlist_shift(list);

	CMNode *expr = cm_parse_expr(list);
	cm_node_append_child(node, expr);

	return node;
}


CMNode *cm_parse_print (CMTokenList *list)
{
	CMNode *node = cm_node(CM_NODE_TYPE_PRINT);

	// discard print
	cm_tokenlist_shift(list);

	CMNode *expr = cm_parse_expr(list);
	cm_node_append_child(node, expr);

	return node;
}


CMNode *cm_parse_file (CMTokenList *list)
{
	CMNode *root = cm_node(CM_NODE_TYPE_ROOT);

	while (! cm_tokenlist_empty(*list)) {
		CMToken token = cm_tokenlist_first(*list);

		switch (token.type) {
			case CM_TOKEN_TYPE_WORD: {
				CMTokenType fmt_assignment[] = {
					CM_TOKEN_TYPE_WORD,
					CM_TOKEN_TYPE_COLON_EQ,
				};

				if (! cm_tokenlist_like(*list, fmt_assignment)) {
					cm_syntax_error(token, "A statement that begins with a word must be a definition");
				}

				cm_node_append_child(
					root,
					cm_parse_symbol_def(list)
				);

				break;
			}

			case CM_TOKEN_TYPE_COLON: {
				cm_node_append_child(
					root,
					cm_parse_print(list)
				);

				break;
			}

			case CM_TOKEN_TYPE_ENDL: {
				cm_tokenlist_shift(list);
				break;
			}

			default: {
				cm_syntax_error(token, "Invalid begin of statement");
			}
		}
	}

	return root;
}

#endif