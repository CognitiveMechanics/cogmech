
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
	CM_NODE_TYPE_COUNT
} CMNodeType;


typedef struct CMNode {
	CMNodeType type;
	struct CMNode **children;
	size_t n_children;
	size_t cap;
	CMStringView value;
} CMNode;


const char *CM_NODE_TYPES_READABLE[CM_NODE_TYPE_COUNT] = {
	"CM_NODE_TYPE_ROOT",
	"CM_NODE_TYPE_SYMBOL_DEF",
	"CM_NODE_TYPE_LITERAL",
	"CM_NODE_TYPE_SYMBOL",
	"CM_NODE_TYPE_COMPOSE",
	"CM_NODE_TYPE_EXTRACT",
	"CM_NODE_TYPE_TRANSCLUDE",
	"CM_NODE_TYPE_MATCH",
	"CM_NODE_TYPE_PRINT",
	"CM_NODE_TYPE_NULL",
	"CM_NODE_TYPE_TRUE",
	"CM_NODE_TYPE_PROXY",
	"CM_NODE_TYPE_DOT_PROXY",
	"CM_NODE_TYPE_DOT",
	"CM_NODE_TYPE_KEY",
	"CM_NODE_TYPE_RELATION_DEF",
	"CM_NODE_TYPE_EVAL",
	"CM_NODE_TYPE_OP_DEF",
	"CM_NODE_TYPE_OP_ARGLIST",
	"CM_NODE_TYPE_OP_INVOKE",
};


const char* CM_NODE_TYPE_WORDS[CM_NODE_TYPE_COUNT] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"T",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"key",
	NULL,
	"R",
	NULL,
	NULL,
	NULL,
};


static const CMTokenType CM_FMT_ASSIGNMENT[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_COLON_EQ,
};

static const CMTokenType CM_FMT_RELATION[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_D_ARROW,
};

static const CMTokenType CM_FMT_RELATION_ALIASED[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_COLON,
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_D_ARROW,
};

static const CMTokenType CM_FMT_OP_START[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_PAREN_IN,
};


const char *cm_readable_node_type (CMNodeType type)
{
	return CM_NODE_TYPES_READABLE[type];
}


bool cm_node_type_has_value (CMNodeType type)
{
	switch (type) {
		case CM_NODE_TYPE_LITERAL:
		case CM_NODE_TYPE_SYMBOL: {
			return true;
		}

		default: {
			return false;
		}
	}
}


void cm_node_alloc_children (CMNode *node, size_t cap)
{
	node->children = calloc(cap, sizeof(CMNode *));

	assert(node->children != NULL);
	assert(node->children[0] == NULL);

	node->cap = cap;
}


CMNode *cm_node (CMNodeType type)
{
	CMNode* node = calloc(1, sizeof(CMNode));

	node->type = type;
	node->n_children = 0;
	node->value = CM_SV_NULL;

	cm_node_alloc_children(node, CM_NODE_CHILDREN_BLOCK_SIZE);

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


CMNode *cm_node_null (void)
{
	return cm_node(CM_NODE_TYPE_NULL);
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


CMNode *cm_node_clone (CMNode *node)
{
	CMNode *clone = cm_node(node->type);

	clone->n_children = 0;
	clone->value = node->value;

	cm_node_alloc_children(clone, node->cap);

	for (size_t i = 0; i < node->n_children; i++) {
		cm_node_append_child(clone, cm_node_clone(node->children[i]));
	}

	return clone;
}


bool cm_node_eq (const CMNode *node1, const CMNode *node2)
{
	if (node1->type != node2->type) {
		return false;
	}

	if (node1->n_children != node2->n_children) {
		return false;
	}

	if (cm_node_type_has_value(node1->type)) {
		if (!cm_sv_eq(node1->value, node2->value)) {
			return false;
		}
	}

	for (size_t i = 0; i < node1->n_children; i++) {
		if (! cm_node_eq(node1->children[i], node2->children[i])) {
			return false;
		}
	}

	return true;
}


CMNode *cm_node_from_word (CMStringView name)
{
	for (size_t i = 1; i < ARRAY_LEN(CM_NODE_TYPE_WORDS); i++) {
		const char *cstr = CM_NODE_TYPE_WORDS[i];

		if (cstr) {
			if (cm_sv_eq(name, cm_sv(cstr))) {
				return cm_node(i);
			}
		}
	}

	return NULL;
}


void _cm_print_node (CMNode *node, int indent_level, int num_spaces)
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
		(int) node->value.len,
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


CMNode *cm_parse_compose (CMTokenList *list);
CMNode *cm_parse_expr (CMTokenList *list);


CMNode *cm_parse_extract (CMTokenList *list)
{
	CMToken token = cm_tokenlist_shift(list); // shift word
	CMNode *symbol = cm_node_symbol(token.value);

	CMNode *extraction = cm_node(CM_NODE_TYPE_EXTRACT);
	cm_node_append_child(extraction, symbol);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_IN); // shift [
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_tokenlist_skip_endl(list);

	cm_node_append_child(extraction, expr);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_OUT); // shift ]

	return extraction;
}


CMNode *cm_parse_expr_list (CMTokenList *list, CMNodeType node_type, CMTokenType start_token_type, CMTokenType end_token_type)
{
	CMNode *node = cm_node(node_type);
	cm_tokenlist_expect(list, start_token_type);

	do {
		cm_tokenlist_skip_endl(list);

		cm_node_append_child(
			node,
			cm_parse_expr(list)
		);

		cm_tokenlist_skip_endl(list);

		CMToken next_token = cm_tokenlist_first(*list);

		if (next_token.type == CM_TOKEN_TYPE_COMMA) {
			cm_tokenlist_shift(list);
			continue;
		} else {
			break;
		}
	} while (! cm_tokenlist_empty(*list));

	cm_tokenlist_skip_endl(list);
	cm_tokenlist_expect(list, end_token_type);

	return node;
}


CMNode *cm_parse_transclude (CMTokenList *list)
{
	CMToken op = cm_tokenlist_shift(list);

	assert(op.type == CM_TOKEN_TYPE_WORD);
	assert(cm_sv_eq(op.value, cm_sv(CM_NODE_TYPE_WORDS[CM_NODE_TYPE_TRANSCLUDE])));
	assert(list->len >= 4);

	CMNode *new_node = cm_parse_expr_list(list, CM_NODE_TYPE_TRANSCLUDE, CM_TOKEN_TYPE_PAREN_IN, CM_TOKEN_TYPE_PAREN_OUT);

	if (new_node->n_children != 3) {
		cm_syntax_error(op, "Transclude accepts exactly 3 arguments");
	}

	CMNode *entity = new_node->children[0];

	// TODO: should be syntax error
	assert(
		(entity->type == CM_NODE_TYPE_COMPOSE || entity->type == CM_NODE_TYPE_SYMBOL)
		&& "Transcluded nodes must be composed entities or symbols"
	);

	return new_node;
}


CMNode *cm_parse_match (CMTokenList *list)
{
	cm_tokenlist_shift(list); // shift %
	CMToken first_token = cm_tokenlist_first(*list);

	CMNode *match = cm_parse_expr_list(list, CM_NODE_TYPE_MATCH, CM_TOKEN_TYPE_PAREN_IN, CM_TOKEN_TYPE_PAREN_OUT);

	if (match->n_children != 2) {
		cm_syntax_error(first_token, "match accepts exactly two arguments");
	}

	return match;
}


CMNode *cm_parse_eval (CMTokenList *list)
{
	CMToken word = cm_tokenlist_expect(list, CM_TOKEN_TYPE_WORD);

	if (! cm_sv_eq(word.value, cm_sv(CM_NODE_TYPE_WORDS[CM_NODE_TYPE_EVAL]))) {
		assert(false && "Invalid eval word");
	}

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_IN);
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_tokenlist_skip_endl(list);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_OUT);

	CMNode *eval = cm_node(CM_NODE_TYPE_EVAL);
	cm_node_append_child(eval, expr);

	return eval;
}


CMNode *cm_parse_op_invoke (CMTokenList *list)
{
	if (! cm_tokenlist_like(*list, CM_FMT_OP_START)) {
		assert(false && "Invalid token list for op definition");
	}

	CMToken symbol_token = cm_tokenlist_shift(list);

	CMNode *def = cm_parse_expr_list(
		list,
		CM_NODE_TYPE_OP_INVOKE,
		CM_TOKEN_TYPE_PAREN_IN,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	def->value = symbol_token.value;

	return def;
}


CMNode *cm_parse_builtin (CMTokenList *list)
{
	CMToken token = cm_tokenlist_first(*list);
	CMNode *builtin = cm_node_from_word(token.value);

	assert(builtin);

	switch (builtin->type) {
		case CM_NODE_TYPE_TRANSCLUDE: {
			return cm_parse_transclude(list);
		}

		case CM_NODE_TYPE_EVAL: {
			return cm_parse_eval(list);
		}

		default: {
			assert(false && "Invalid builtin word");
		}
	}
}


CMNode *cm_parse_class (CMTokenList *list)
{
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_IN);
	bool is_dot = false;

	if (cm_tokenlist_first_like(*list, CM_TOKEN_TYPE_DOT)) {
		cm_tokenlist_shift(list);
		is_dot = true;
	}

	CMToken word = cm_tokenlist_expect(list, CM_TOKEN_TYPE_WORD);
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_OUT);

	CMNode *composition = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(composition, cm_node_literal(word.value));

	if (is_dot) {
		cm_node_append_child(composition, cm_node(CM_NODE_TYPE_DOT_PROXY));
	} else {
		cm_node_append_child(composition, cm_node(CM_NODE_TYPE_PROXY));
	}

	return composition;
}


CMNode *cm_parse_key (CMTokenList *list)
{
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_HASH);
	CMNode *expr = cm_parse_expr(list);

	CMNode *composition = cm_node(CM_NODE_TYPE_COMPOSE);

	cm_node_append_child(composition, cm_node(CM_NODE_TYPE_KEY));
	cm_node_append_child(composition, expr);

	return composition;
}


CMNode *cm_parse_dot (CMTokenList *list)
{
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_DOT);
	CMToken word = cm_tokenlist_expect(list, CM_TOKEN_TYPE_WORD);

	CMNode *dot = cm_node(CM_NODE_TYPE_DOT);
	cm_node_append_child(dot, cm_node_symbol(word.value));

	return dot;
}


CMNode *cm_parse_expr (CMTokenList *list)
{
	cm_tokenlist_skip_endl(list);
	CMToken token = cm_tokenlist_first(*list);

	switch (token.type) {
		case CM_TOKEN_TYPE_WORD: {
			CMNode *builtin = cm_node_from_word(token.value);

			if (builtin) {
				return cm_parse_builtin(list);
			}

			if (list->len > 1) {
				CMToken next_token = cm_tokenlist_get(*list, 1);

				if (next_token.type == CM_TOKEN_TYPE_SQ_BRACKET_IN) {
					return cm_parse_extract(list);
				}
			}

			if (cm_tokenlist_like(*list, CM_FMT_OP_START)) {
				return cm_parse_op_invoke(list);
			}

			cm_tokenlist_shift(list); // shift word
			return cm_node_symbol(token.value);
		}

		case CM_TOKEN_TYPE_DOT: {
			return cm_parse_dot(list);
		}

		case CM_TOKEN_TYPE_SQ_BRACKET_IN: {
			return cm_parse_class(list);
		}

		case CM_TOKEN_TYPE_HASH: {
			return cm_parse_key(list);
		}

		case CM_TOKEN_TYPE_QUOTED: {
			cm_tokenlist_shift(list);
			return cm_node_literal(token.value);
		}

		case CM_TOKEN_TYPE_LT: {
			return cm_parse_compose(list);
		}

		case CM_TOKEN_TYPE_PROXY: {
			cm_tokenlist_shift(list);
			return cm_node(CM_NODE_TYPE_PROXY);
		}

		case CM_TOKEN_TYPE_DOT_PROXY: {
			cm_tokenlist_shift(list);
			return cm_node(CM_NODE_TYPE_DOT_PROXY);
		}

		case CM_TOKEN_TYPE_TRUE: {
			cm_tokenlist_shift(list);
			return cm_node(CM_NODE_TYPE_TRUE);
		}

		case CM_TOKEN_TYPE_NULL: {
			cm_tokenlist_shift(list);
			return cm_node_null();
		}

		case CM_TOKEN_TYPE_PERCENT: {
			return cm_parse_match(list);
		}

		default: {
			cm_syntax_error(token, "Expected expression");
		}
	}

	assert(false && "Unreachable");
}


CMNode *cm_parse_compose (CMTokenList *list)
{
	return cm_parse_expr_list(list, CM_NODE_TYPE_COMPOSE, CM_TOKEN_TYPE_LT, CM_TOKEN_TYPE_GT);
}


CMNode *cm_parse_symbol_def (CMTokenList *list)
{
	CMNode *node = cm_node(CM_NODE_TYPE_SYMBOL_DEF);

	if (! cm_tokenlist_like(*list, CM_FMT_ASSIGNMENT)) {
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


CMNode *cm_parse_op_def (CMTokenList *list)
{
	if (! cm_tokenlist_like(*list, CM_FMT_OP_START)) {
		assert(false && "Invalid token list for op definition");
	}

	CMToken symbol_token = cm_tokenlist_shift(list);

	CMNode *def = cm_node(CM_NODE_TYPE_OP_DEF);
	def->value = symbol_token.value;

	CMNode *arglist = cm_parse_expr_list(
		list,
		CM_NODE_TYPE_OP_ARGLIST,
		CM_TOKEN_TYPE_PAREN_IN,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	for (size_t i = 0; i < def->n_children; i++) {
		if (def->children[i]->type != CM_NODE_TYPE_SYMBOL) {
			cm_syntax_error(symbol_token, "All arguments in op definition must be symbols");
		}
	}

	cm_node_append_child(def, arglist);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_S_ARROW);
	cm_node_append_child(def, cm_parse_expr(list));

	return def;
}


CMNode *cm_parse_relation_def (CMTokenList *list)
{
	bool is_relation = cm_tokenlist_like(*list, CM_FMT_RELATION);
	bool is_aliased_relation = cm_tokenlist_like(*list, CM_FMT_RELATION_ALIASED);

	assert(is_relation || is_aliased_relation);

	CMNode *relation = cm_node(CM_NODE_TYPE_RELATION_DEF);

	CMNode *state = cm_parse_expr(list);
	cm_node_append_child(relation, state);

	if (is_aliased_relation) {
		cm_tokenlist_expect(list, CM_TOKEN_TYPE_COLON); // discard :
		CMToken word = cm_tokenlist_expect(list, CM_TOKEN_TYPE_WORD);
		CMNode *symbol = cm_node_symbol(word.value);
		cm_node_append_child(relation, symbol);
	} else {
		cm_node_append_child(relation, cm_node_null());
	}

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_D_ARROW); // discard =>

	CMNode *expr = cm_parse_expr(list);
	cm_node_append_child(relation, expr);

	return relation;
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


CMNode *cm_parse (CMTokenList *list)
{
	assert(CM_NODE_TYPE_COUNT == 20);
	assert(CM_TOKEN_TYPE_COUNT == 22);

	CMNode *root = cm_node(CM_NODE_TYPE_ROOT);

	while (! cm_tokenlist_empty(*list)) {
		CMToken token = cm_tokenlist_first(*list);

		switch (token.type) {
			case CM_TOKEN_TYPE_WORD: {
				bool is_assignment = cm_tokenlist_like(*list, CM_FMT_ASSIGNMENT);

				if (is_assignment) {
					cm_node_append_child(
						root,
						cm_parse_symbol_def(list)
					);

					break;
				}

				bool is_relation = cm_tokenlist_like(*list, CM_FMT_RELATION);
				bool is_aliased_relation = cm_tokenlist_like(*list, CM_FMT_RELATION_ALIASED);

				if (is_relation || is_aliased_relation) {
					cm_node_append_child(
						root,
						cm_parse_relation_def(list)
					);

					break;
				}

				bool is_op = cm_tokenlist_like(*list, CM_FMT_OP_START);

				if (is_op) {
					cm_node_append_child(
						root,
						cm_parse_op_def(list)
					);

					break;
				}

				cm_syntax_error(token, "A statement that begins with a word must be a definition");
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
				cm_tokenlist_skip_endl(list);
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

