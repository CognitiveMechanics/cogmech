
#ifndef CM_PARSER_H
#define CM_PARSER_H

#include <stdlib.h>
#include <stdio.h>

#include "tokenizer.h"


typedef enum CMNodeType {
	CM_NODE_TYPE_ROOT = 0,
	CM_NODE_TYPE_SYMBOL_DEF,
	CM_NODE_TYPE_LITERAL,
	CM_NODE_TYPE_SYMBOL,
	CM_NODE_TYPE_COMPOSITION,
	CM_NODE_TYPE_PRINT,
	CM_NODE_TYPE_COUNT
} CMNodeType;


typedef struct CMNode {
	CMNodeType type;
	size_t n_children;
	struct CMNode *children;
} CMNode;


void cm_node_alloc_children (CMNode *node, size_t n)
{
	node->children = malloc(n);
	assert(node->children != NULL);

	node->n_children = n;
}


void cm_node_free_children (CMNode *node)
{
	assert(node->children != NULL);

	for (size_t i = 0; i <= node->n_children; i++) {
		if (node->children[i].children != NULL) {
			cm_node_free_children(&node->children[i]);
		}
	}

	free(node->children);

	node->n_children = 0;
}


CMNode cm_parse_file (CMTokenList *list)
{
	CMNode root = {0};

	while (! cm_tokenlist_empty(*list)) {
		CMToken token = cm_tokenlist_get(*list, 0);

		switch (token.type) {
			case CM_TOKEN_TYPE_WORD: {

				break;
			}
			case CM_TOKEN_TYPE_COLON: {
				break;
			}

			default: {
				char error_message[50];
				sprintf(error_message, "%s cannot begin a statement\n", cm_readable_token_type(token.type));
				cm_syntax_error(token, error_message);
			}
		}
	}
}


CMNode cm_parse_assignment (CMTokenList *list)
{
	CMTokenType fmt_assign_to_symbol[] = {
		CM_TOKEN_TYPE_WORD,
		CM_TOKEN_TYPE_COLON_EQ,
		CM_TOKEN_TYPE_WORD
	};
}

#endif