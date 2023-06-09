
#include "node.h"
#include <assert.h>


static const char *CM_NODE_TYPES_READABLE[CM_NODE_TYPE_COUNT] = {
	"CM_NODE_TYPE_ROOT",
	"CM_NODE_TYPE_SYMBOL_DEF",
	"CM_NODE_TYPE_LITERAL",
	"CM_NODE_TYPE_SYMBOL",
	"CM_NODE_TYPE_INT",
	"CM_NODE_TYPE_INT_EXACT",
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
	"CM_NODE_TYPE_INCREMENT",
	"CM_NODE_TYPE_DECREMENT",
	"CM_NODE_TYPE_INCLUDE",
};


static const char* CM_NODE_TYPE_WORDS[CM_NODE_TYPE_COUNT] = {
	NULL,
	NULL,
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
	NULL,
	NULL,
	NULL,
};


const char *cm_node_type_readable (CMNodeType type)
{
	return CM_NODE_TYPES_READABLE[type];
}


const char *cm_node_type_word (CMNodeType type)
{
	return CM_NODE_TYPE_WORDS[type];
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


bool cm_node_type_has_int_value (CMNodeType type)
{
	switch (type) {
		case CM_NODE_TYPE_INT:
		case CM_NODE_TYPE_INT_EXACT: {
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
	node->token = CM_TOKEN_NULL;

	cm_node_alloc_children(node, CM_NODE_CHILDREN_BLOCK_SIZE);

	return node;
}


void cm_node_set_token (CMNode *node, CMToken token)
{
	node->token = token;
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


CMNode *cm_node_int (CMStringView value)
{
	CMNode *node = cm_node(CM_NODE_TYPE_INT_EXACT);
	node->type = CM_NODE_TYPE_INT;
	node->value = value;

	return node;
}


CMNode *cm_node_int_exact (CMStringView value)
{
	CMNode *node = cm_node_int(value);
	node->type = CM_NODE_TYPE_INT_EXACT;

	return node;
}


CMNode *cm_node_int_from_int (cm_int value)
{
	char *buffer = calloc(16, sizeof(char));
	snprintf(buffer, 16, "%ld", value);

	return cm_node_int(cm_sv(buffer));
}


CMNode *cm_node_int_exact_from_int (cm_int value)
{
	CMNode *node = cm_node_int_from_int(value);
	node->type = CM_NODE_TYPE_INT_EXACT;

	return node;
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
	clone->token = node->token;

	cm_node_alloc_children(clone, node->cap);

	for (size_t i = 0; i < node->n_children; i++) {
		cm_node_append_child(clone, cm_node_clone(node->children[i]));
	}

	return clone;
}


cm_int cm_node_int_value (const CMNode *node)
{
	assert(node->type == CM_NODE_TYPE_INT || node->type == CM_NODE_TYPE_INT_EXACT);

	return strtol(cm_sv_to_cstr(node->value), NULL, 10);
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
		if (! cm_sv_eq(node1->value, node2->value)) {
			return false;
		}
	}

	if (cm_node_type_has_int_value(node1->type)) {
		if (cm_node_int_value(node1) != cm_node_int_value(node2)) {
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
		cm_node_type_readable(node->type),
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
			"%*s  }\n",
			indent_level * num_spaces,
			""
		);
	} else {
		printf("}\n");
	}

//	printf(
//		"%*s  .token =\n",
//		indent_level * num_spaces,
//		""
//	);
//
//	cm_print_token_indented(node->token, indent_level + 1, num_spaces);

	printf(
		"%*s}\n",
		indent_level * num_spaces,
		""
	);
}


void cm_print_node (CMNode *node)
{
	_cm_print_node(node, 0, 4);
}
