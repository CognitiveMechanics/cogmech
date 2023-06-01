
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


typedef struct CMNode {
	CMNodeType type;
	struct CMNode **children;
	size_t n_children;
	size_t cap;
} CMNode;


void cm_node_alloc_children (CMNode *node)
{
	node->children = malloc(CM_NODE_CHILDREN_BLOCK_SIZE);
	assert(node->children != NULL);

	node->cap = CM_NODE_CHILDREN_BLOCK_SIZE;
}


CMNode *cm_node (CMNodeType type)
{
	CMNode* node = malloc(sizeof(CMNode));
	node->type = type;

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

	return root;
}


CMNode cm_parse_assignment (CMTokenList *list)
{
	CMNode node = {0};

	CMTokenType fmt_assign_to_symbol[] = {
		CM_TOKEN_TYPE_WORD,
		CM_TOKEN_TYPE_COLON_EQ,
		CM_TOKEN_TYPE_WORD
	};

	return node;
}

#endif