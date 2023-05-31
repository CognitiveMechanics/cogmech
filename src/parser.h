
#ifndef CM_PARSER_H
#define CM_PARSER_H

#include <stdlib.h>
#include <stdio.h>

#include "tokenizer.h"


typedef enum CMNodeType {
	CM_NODE_TYPE_ROOT = 0,
	CM_NODE_TYPE_ENTITY,
	CM_NODE_TYPE_REF_DEF,
	CM_NODE_TYPE_OP_INVOCATION,
	CM_NODE_TYPE_PRINT,
	CM_NODE_TYPE_COUNT
} CMNodeType;


typedef enum CMOpType {
	CM_OP_TYPE_COMPOSE = 0,
	CM_OP_TYPE_COUNT
} CMOpType;


typedef struct CMNode {
	CMNodeType type;
	size_t n_children;
	struct CMNode *children;
} CMNode;


void cm_alloc_node_children (CMNode *node, size_t n)
{
	node->children = malloc(n);
	assert(node->children != NULL);

	node->n_children = n;
}


CMNode cm_parse_tokenlist (CMTokenList list)
{
	for (size_t i = 0; i < list.len; i++) {
		CMToken token = cm_tokenlist_get(list, i);

		if (token_queue.len == 0) {
			switch (token.type) {
				case CM_TOKEN_TYPE_WORD:
				case CM_TOKEN_TYPE_COLON: {
					cm_tokenlist_append(&token_queue, token);
					break;
				}

				default: {
					char error_message[50];
					sprintf(error_message, "%s cannot begin a statement\n", cm_readable_token_type(token.type));
					cm_syntax_error(token, error_message);
				}
			}
		} else {
			CMToken stmt_first_token = cm_tokenlist_get(token_queue, 0);

			if (stmt_first_token.type == CM_TOKEN_TYPE_WORD) {

			}
		}
	}

	cm_tokenlist_free(&token_queue);
}

#endif