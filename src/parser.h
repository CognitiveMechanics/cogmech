
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
	CM_NODE_TYPE_INT,
	CM_NODE_TYPE_INT_EXACT,
	CM_NODE_TYPE_COMPOSE,
	CM_NODE_TYPE_EXTRACT,
	CM_NODE_TYPE_TRANSCLUDE,
	CM_NODE_TYPE_MATCH,
	CM_NODE_TYPE_PRINT,
	CM_NODE_TYPE_NULL,
	CM_NODE_TYPE_TRUE,
	CM_NODE_TYPE_PROXY,
	CM_NODE_TYPE_DOT_PROXY,
	CM_NODE_TYPE_DOT,
	CM_NODE_TYPE_KEY,
	CM_NODE_TYPE_RELATION_DEF,
	CM_NODE_TYPE_EVAL,
	CM_NODE_TYPE_OP_DEF,
	CM_NODE_TYPE_OP_ARGLIST,
	CM_NODE_TYPE_OP_INVOKE,
	CM_NODE_TYPE_INCREMENT,
	CM_NODE_TYPE_DECREMENT,
	CM_NODE_TYPE_COUNT
} CMNodeType;


typedef struct CMNode {
	CMNodeType type;
	struct CMNode **children;
	size_t n_children;
	size_t cap;
	CMStringView value;
	CMToken token;
} CMNode;


typedef long cm_int;


const char *cm_node_type_readable (CMNodeType type);
const char *cm_node_type_word (CMNodeType type);
bool cm_node_type_has_value (CMNodeType type);
bool cm_node_type_has_int_value (CMNodeType type);

CMNode *cm_node (CMNodeType type);
void cm_node_free (CMNode *node);
CMNode *cm_node_clone (CMNode *node);
void cm_node_set_token (CMNode *node, CMToken token);

CMNode *cm_node_literal (CMStringView name);
CMNode *cm_node_symbol (CMStringView identifier);
CMNode *cm_node_int (CMStringView value);
CMNode *cm_node_int_exact (CMStringView value);
CMNode *cm_node_int_from_int (cm_int value);
CMNode *cm_node_int_exact_from_int (cm_int value);
CMNode *cm_node_from_word (CMStringView name);
CMNode *cm_node_null (void);

void cm_node_append_child (CMNode *node, CMNode *child);
bool cm_node_eq (const CMNode *node1, const CMNode *node2);

void cm_print_node (CMNode *node);
cm_int cm_node_int_value (const CMNode *node);

CMNode *cm_parse_compose (CMTokenList *list);
CMNode *cm_parse_word (CMTokenList *list);
CMNode *cm_parse_int (CMTokenList *list);
CMNode *cm_parse_expr (CMTokenList *list);
CMNode *cm_parse_increment (CMTokenList *list);
CMNode *cm_parse_decrement (CMTokenList *list);
CMNode *cm_parse_expr_list (CMTokenList *list, CMNodeType node_type, CMTokenType start_token_type, CMTokenType end_token_type);
CMNode *cm_parse_extract (CMTokenList *list);
CMNode *cm_parse_transclude (CMTokenList *list);
CMNode *cm_parse_match (CMTokenList *list);
CMNode *cm_parse_eval (CMTokenList *list);
CMNode *cm_parse_op_invoke (CMTokenList *list);
CMNode *cm_parse_builtin (CMTokenList *list);
CMNode *cm_parse_class (CMTokenList *list);
CMNode *cm_parse_key (CMTokenList *list);
CMNode *cm_parse_dot (CMTokenList *list);
CMNode *cm_parse_expr (CMTokenList *list);
CMNode *cm_parse_compose (CMTokenList *list);
CMNode *cm_parse_symbol_def (CMTokenList *list);
CMNode *cm_parse_op_def (CMTokenList *list);
CMNode *cm_parse_relation_def (CMTokenList *list);
CMNode *cm_parse_print (CMTokenList *list);
CMNode *cm_parse (CMTokenList *list);

#endif

