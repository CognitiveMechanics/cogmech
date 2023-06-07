

#include "unit.h"
#include "test_parser.h"
#include "../../src/parser.h"


bool test_cm_node_type_has_value (void)
{
	if (! cm_node_type_has_value(CM_NODE_TYPE_SYMBOL)) {
		cm_test_error("CM_NODE_TYPE_SYMBOL should have value\n");
		return false;
	}

	if (! cm_node_type_has_value(CM_NODE_TYPE_LITERAL)) {
		cm_test_error("CM_NODE_TYPE_LITERAL should have value\n");
		return false;
	}

	if (cm_node_type_has_value(CM_NODE_TYPE_PRINT)) {
		cm_test_error("CM_NODE_TYPE_PRINT should not have value\n");
		return false;
	}

	return true;
}


bool test_cm_node_type_has_int_value (void)
{
	if (! cm_node_type_has_int_value(CM_NODE_TYPE_INT)) {
		cm_test_error("CM_NODE_TYPE_INT should have value\n");
		return false;
	}

	if (! cm_node_type_has_int_value(CM_NODE_TYPE_INT_EXACT)) {
		cm_test_error("CM_NODE_TYPE_INT_EXACT should have value\n");
		return false;
	}

	if (cm_node_type_has_int_value(CM_NODE_TYPE_SYMBOL)) {
		cm_test_error("CM_NODE_TYPE_PRINT should not have value\n");
		return false;
	}

	return true;
}


bool test_cm_node_alloc_free (void)
{
	CMNode *parent = cm_node(CM_NODE_TYPE_ROOT);

	if (parent->children == NULL) {
		cm_test_error("failed to allocate parent children\n");
		return false;
	}

	if (parent->n_children != 0) {
		cm_test_error("parent node indicates children before any have been appended\n");
		return false;
	}

	CMNode *child = cm_node(CM_NODE_TYPE_SYMBOL);

	if (child->children == NULL) {
		cm_test_error("failed to allocate child children\n");
		return false;
	}

	if (child->n_children != 0) {
		cm_test_error("child node indicates children before any have been appended\n");
		return false;
	}

	CMNode *grandchild = cm_node(CM_NODE_TYPE_LITERAL);

	cm_node_append_child(child, grandchild);
	cm_node_append_child(parent, child);

	if (child->n_children != 1) {
		cm_test_error("child node indicates no children\n");
		return false;
	}

	if (parent->n_children != 1) {
		cm_test_error("parent node indicates no children\n");
		return false;
	}

	cm_node_free(parent);

	if (child->n_children != 0) {
		cm_test_error("child still indicates children after parent has been freed\n");
		return false;
	}

	if (parent->n_children != 0) {
		cm_test_error("parent still indicates children after it has been freed\n");
		return false;
	}

	return true;
}


bool test_cm_node_set_token (void)
{
	CMToken token = cm_token("test.cogm", 1, 2, CM_TOKEN_TYPE_WORD);
	CMNode *node = cm_node_symbol(cm_sv("symbol"));
	cm_node_set_token(node, token);

	if (node->token.type != CM_TOKEN_TYPE_WORD) {
		cm_test_error("invalid token type\n");
		return false;
	}

	if (node->token.loc.row != 1) {
		cm_test_error("invalid row\n");
		return false;
	}

	if (node->token.loc.col != 2) {
		cm_test_error("invalid col\n");
		return false;
	}

	if (strcmp(node->token.loc.filename, "test.cogm")) {
		cm_test_error("invalid filename\n");
		return false;
	}

	return true;
}


bool test_cm_node_symbol (void)
{
	CMStringView sv = cm_sv("sym");
	CMNode *node = cm_node_symbol(sv);

	if (node->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (! cm_sv_eq(node->value, cm_sv("sym"))) {
		cm_test_error("failed to verify value is 'sym'\n");
		return false;
	}

	return true;
}


bool test_cm_node_literal (void)
{
	CMStringView sv = cm_sv("lit");
	CMNode *node = cm_node_symbol(sv);

	if (node->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (! cm_sv_eq(node->value, cm_sv("lit"))) {
		cm_test_error("failed to verify value is 'lit'\n");
		return false;
	}

	return true;
}


bool test_cm_node_int (void)
{
	CMStringView sv = cm_sv("1");
	CMNode *node = cm_node_int(sv);

	if (node->type != CM_NODE_TYPE_INT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (! cm_sv_eq(node->value, cm_sv("1"))) {
		cm_test_error("failed to verify value is '1'\n");
		return false;
	}

	return true;
}


bool test_cm_node_int_exact (void)
{
	CMStringView sv = cm_sv("1");
	CMNode *node = cm_node_int_exact(sv);

	if (node->type != CM_NODE_TYPE_INT_EXACT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (! cm_sv_eq(node->value, cm_sv("1"))) {
		cm_test_error("failed to verify value is '1'\n");
		return false;
	}

	return true;
}


bool test_cm_node_int_from_int (void)
{
	CMNode *node = cm_node_int_from_int(1);

	if (node->type != CM_NODE_TYPE_INT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (cm_node_int_value(node) != 1) {
		cm_test_error("invalid node value\n");
		return false;
	}

	return true;
}


bool test_cm_node_int_exact_from_int (void)
{
	CMNode *node = cm_node_int_exact_from_int(1);

	if (node->type != CM_NODE_TYPE_INT_EXACT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (cm_node_int_value(node) != 1) {
		cm_test_error("invalid node value\n");
		return false;
	}

	return true;
}


bool test_cm_node_int_value (void)
{
	CMNode *int_node = cm_node_int(cm_sv("123"));
	CMNode *exact_node = cm_node_int_exact(cm_sv("456"));

	if (cm_node_int_value(int_node) != 123) {
		cm_test_error("invalid int value\n");
		return false;
	}

	if (cm_node_int_value(exact_node) != 456) {
		cm_test_error("invalid exact int value\n");
		return false;
	}

	return true;
}


bool test_cm_node_eq (void)
{
	CMTokenList seq1 = cm_tokenize("test1.cogm", cm_sv(" a  := <\"b\", c>\n"));
	CMTokenList seq2 = cm_tokenize("test2.cogm", cm_sv("a := < \"b\" , c > \n"));
	CMTokenList seq3 = cm_tokenize("test3.cogm", cm_sv("a := <\"b\", x>\n"));

	CMNode *ast1 = cm_parse(&seq1);
	CMNode *ast2 = cm_parse(&seq2);
	CMNode *ast3 = cm_parse(&seq3);

	if (! cm_node_eq(ast1, ast1)) {
		cm_test_error("identical nodes should be equal\n");
		return false;
	}

	if (! cm_node_eq(ast1, ast2)) {
		cm_test_error("same nodes should be equal\n");
		return false;
	}

	if (! cm_node_eq(ast2, ast1)) {
		cm_test_error("same nodes reversed should be equal\n");
		return false;
	}

	if (cm_node_eq(ast1, ast3)) {
		cm_test_error("different nodes should not be equal\n");
		return false;
	}

	return true;
}


bool test_cm_node_from_word (void)
{
	CMNode *null_node_1 = cm_node_from_word(cm_sv(":"));
	CMNode *null_node_2 = cm_node_from_word(cm_sv("O"));
	CMNode *transclude_node = cm_node_from_word(cm_sv(cm_node_type_word(CM_NODE_TYPE_TRANSCLUDE)));

	if (null_node_1 || null_node_2) {
		cm_test_error("found node with unknown word\n");
		return false;
	}

	if (! transclude_node) {
		cm_test_error("transclude node not found\n");
		return false;
	}

	if (transclude_node->type != CM_NODE_TYPE_TRANSCLUDE) {
		cm_test_error("invalid transclude node type\n");
		return false;
	}

	return true;
}


bool test_cm_node_clone (void)
{
	CMTokenList seq1 = cm_tokenize("test1.cogm", cm_sv(" a  := <\"b\", c>\n"));

	CMNode *node1 = cm_parse(&seq1);
	CMNode *node2 = node1;
	CMNode *node3 = cm_node_clone(node1);

	node2->children[0]->children[0]->value = cm_sv("d");
	node3->children[0]->children[0]->value = cm_sv("e");

	if (! cm_node_eq(node1, node2)) {
		cm_test_error("expected copies to be the same");
		return false;
	}

	if (cm_node_eq(node1, node3)) {
		cm_print_node(node1);
		cm_test_error("expected clones to be the different");
		return false;
	}

	return true;
}


bool test_cm_parse_compose (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_LT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_LT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_GT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_GT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_expr(&list);

	if (composition->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("invalid composition expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of composition should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (composition->children[2]->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("third element of composition should be CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (composition->children[2]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("inner element of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_extract (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_extract(&list);

	if (composition->type != CM_NODE_TYPE_EXTRACT) {
		cm_test_error("invalid extract expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of composition should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_transclude (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);
	keyword.value = cm_sv("T");

	CMToken tokens[] = {
		keyword,
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_transclude(&list);

	if (composition->type != CM_NODE_TYPE_TRANSCLUDE) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of transclusion should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (composition->children[2]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("third element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_op_def (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken name = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);
	name.value = cm_sv("O");

	CMToken tokens[] = {
		name,
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_S_ARROW
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *op = cm_parse_op_def(&list);

	if (op->type != CM_NODE_TYPE_OP_DEF) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (! cm_sv_eq(op->value, cm_sv("O"))) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (op->children[0]->type != CM_NODE_TYPE_OP_ARGLIST) {
		cm_test_error("first element of op should be CM_NODE_TYPE_OP_ARGLIST\n");
		return false;
	}

	if (op->children[0]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("arguments should be of type CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (op->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("body of op should be CM_TOKEN_TYPE_WORD\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_op_invoke (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken name = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);
	name.value = cm_sv("O");

	CMToken tokens[] = {
		name,
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *op = cm_parse_op_invoke(&list);

	if (op->type != CM_NODE_TYPE_OP_INVOKE) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (! cm_sv_eq(op->value, cm_sv("O"))) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (op->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of op should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (op->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of op should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (op->children[2]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("third element of op should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_expr_list(void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_expr_list(
		&list,
		CM_NODE_TYPE_TRANSCLUDE,
		CM_TOKEN_TYPE_PAREN_IN,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	if (composition->type != CM_NODE_TYPE_TRANSCLUDE) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of transclusion should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (composition->children[2]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("third element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_match (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PERCENT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_match(&list);

	if (composition->type != CM_NODE_TYPE_MATCH) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of match should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("second element of match should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_class (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_DOT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *parsed = cm_parse_class(&list);

	if (parsed->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("classes should be of type CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (parsed->n_children != 2) {
		cm_test_error("classes should have two children\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("first child should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_DOT_PROXY) {
		cm_test_error("second child should be CM_NODE_TYPE_DOT_PROXY\n");
		return false;
	}

	return true;
}


bool test_cm_parse_key (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_HASH
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *parsed = cm_parse_key(&list);

	if (parsed->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("classes should be of type CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (parsed->n_children != 2) {
		cm_test_error("classes should have two children\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_KEY) {
		cm_test_error("first child should be CM_NODE_TYPE_KEY\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("second child should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	return true;
}


bool test_cm_parse_dot (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_DOT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *parsed = cm_parse_dot(&list);

	if (parsed->type != CM_NODE_TYPE_DOT) {
		cm_test_error("should be of type CM_NODE_TYPE_DOT\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("dot should have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("child should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	return true;
}


bool test_cm_parse_int (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_INT
	);

	token.value = cm_sv("1");

	cm_tokenlist_append(&list, token);

	CMNode *node = cm_parse_int(&list);

	if (node->type != CM_NODE_TYPE_INT) {
		cm_test_error("node should be CM_NODE_TYPE_INT\n");
		return false;
	}

	if (cm_node_int_value(node) != 1) {
		cm_test_error("node value should be 1\n");
		return false;
	}

	return true;
}


bool test_cm_parse_word (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	token.value = cm_sv("test_word");
	cm_tokenlist_append(&list, token);

	CMNode *word = cm_parse_word(&list);

	if (word->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid word type\n");
		return false;
	}

	if (! cm_sv_eq(word->value, cm_sv("test_word"))) {
		cm_test_error("invalid word value\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_expr (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_LT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_GT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *symbol = cm_parse_expr(&list);
	CMNode *literal = cm_parse_expr(&list);
	CMNode *composition = cm_parse_expr(&list);

	if (symbol->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol expression\n");
		return false;
	}

	if (literal->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal expression\n");
		return false;
	}

	if (composition->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("invalid composition expression\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_symbol_def (void)
{
	CMToken symbol = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	symbol.value = cm_sv("a");

	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON_EQ
	);

	CMToken literal = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_QUOTED
	);

	literal.value = cm_sv("string");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, symbol);
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, literal);

	CMNode *parsed = cm_parse_symbol_def(&list);

	if (parsed->type != CM_NODE_TYPE_SYMBOL_DEF) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("a"))) {
		cm_test_error("invalid symbol node value\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->value, cm_sv("string"))) {
		cm_test_error("invalid literal node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse_relation_def (void)
{
	CMToken symbol = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	symbol.value = cm_sv("a");

	CMToken colon = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON
	);

	CMToken state = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	state.value = cm_sv("s0");

	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_D_ARROW
	);

	CMToken literal = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_QUOTED
	);

	literal.value = cm_sv("string");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, symbol);
	cm_tokenlist_append(&list, colon);
	cm_tokenlist_append(&list, state);
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, literal);

	CMNode *parsed = cm_parse_relation_def(&list);

	if (parsed->type != CM_NODE_TYPE_RELATION_DEF) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("a"))) {
		cm_test_error("invalid symbol node value\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid state node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->value, cm_sv("s0"))) {
		cm_test_error("invalid state node value\n");
		return false;
	}

	if (parsed->children[2]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[2]->value, cm_sv("string"))) {
		cm_test_error("invalid literal node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse_eval (void)
{
	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	keyword.value = cm_sv("R");

	CMToken paren_in = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_IN
	);

	CMToken input = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	CMToken paren_out = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, keyword);
	cm_tokenlist_append(&list, paren_in);
	cm_tokenlist_append(&list, input);
	cm_tokenlist_append(&list, paren_out);

	CMNode *parsed = cm_parse_eval(&list);

	if (parsed->type != CM_NODE_TYPE_EVAL) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("node should only have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	return true;
}


bool test_cm_parse_increment (void)
{
	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PLUS
	);

	CMToken paren_in = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_IN
	);

	CMToken input = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	CMToken paren_out = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, keyword);
	cm_tokenlist_append(&list, paren_in);
	cm_tokenlist_append(&list, input);
	cm_tokenlist_append(&list, paren_out);

	CMNode *parsed = cm_parse_increment(&list);

	if (parsed->type != CM_NODE_TYPE_INCREMENT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("node should only have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	return true;
}


bool test_cm_parse_decrement (void)
{
	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_MINUS
	);

	CMToken paren_in = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_IN
	);

	CMToken input = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	CMToken paren_out = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, keyword);
	cm_tokenlist_append(&list, paren_in);
	cm_tokenlist_append(&list, input);
	cm_tokenlist_append(&list, paren_out);

	CMNode *parsed = cm_parse_decrement(&list);

	if (parsed->type != CM_NODE_TYPE_DECREMENT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("node should only have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	return true;
}


bool test_cm_parse_print (void)
{
	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON
	);

	CMToken symbol = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	symbol.value = cm_sv("a");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, symbol);

	CMNode *parsed = cm_parse_print(&list);

	if (parsed->type != CM_NODE_TYPE_PRINT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("a"))) {
		cm_test_error("invalid symbol node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse_include (void)
{
	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_INCLUDE
	);

	CMToken literal = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_QUOTED
	);

	literal.value = cm_sv("include.cogm");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, literal);

	CMNode *parsed = cm_parse_include(&list);

	if (parsed->type != CM_NODE_TYPE_INCLUDE) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("include.cogm"))) {
		cm_test_error("invalid literal node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse (void)
{
	CMTokenList list = cm_tokenize_file("../tests/cogm/examples/00-hello.cogm");
	CMNode *parsed = cm_parse(&list);

	// root

	if (parsed->type != CM_NODE_TYPE_ROOT) {
		cm_test_error("invalid root node type\n");
		return false;
	}

	// a := "hello"

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL_DEF) {
		cm_test_error("invalid assignment of first symbol\n");
		return false;
	}

	if (parsed->children[0]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("left hand of first assignment should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->children[0]->value, cm_sv("a"))) {
		cm_test_error("first symbol should be 'a'\n");
		return false;
	}

	if (parsed->children[0]->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("right hand of first assignment should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->children[1]->value, cm_sv("hello"))) {
		cm_test_error("value of first assignment should be 'hello'\n");
		return false;
	}

	// b := <a, "world">

	if (parsed->children[1]->type != CM_NODE_TYPE_SYMBOL_DEF) {
		cm_test_error("invalid assignment of second symbol\n");
		return false;
	}

	if (parsed->children[1]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("left hand of second assignment should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[0]->value, cm_sv("b"))) {
		cm_test_error("second symbol should be 'b'\n");
		return false;
	}

	if (parsed->children[1]->children[1]->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("right hand of second assignment should be CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (parsed->children[1]->children[1]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first component of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[1]->children[0]->value, cm_sv("a"))) {
		cm_test_error("first component of composition should be 'a'\n");
		return false;
	}

	if (parsed->children[1]->children[1]->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second component of composition should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[1]->children[1]->value, cm_sv("world"))) {
		cm_test_error("second component of composition should be 'world'\n");
		return false;
	}

	// : b

	if (parsed->children[2]->type != CM_NODE_TYPE_PRINT) {
		cm_test_error("invalid print statment\n");
		return false;
	}

	if (parsed->children[2]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("operand of print statement should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[2]->children[0]->value, cm_sv("b"))) {
		cm_test_error("print should be called with 'b'\n");
		return false;
	}

	cm_tokenlist_free(&list);
	cm_node_free(parsed);

	return true;
}


void test_cm_parser (void)
{
	printf("Loading parser tests...\n");

	cm_add_test(test_cm_node_type_has_value);
	cm_add_test(test_cm_node_type_has_int_value);
	cm_add_test(test_cm_node_alloc_free);
	cm_add_test(test_cm_node_set_token);
	cm_add_test(test_cm_node_symbol);
	cm_add_test(test_cm_node_literal);
	cm_add_test(test_cm_node_int);
	cm_add_test(test_cm_node_int_exact);
	cm_add_test(test_cm_node_int_from_int);
	cm_add_test(test_cm_node_int_exact_from_int);
	cm_add_test(test_cm_node_int_value);
	cm_add_test(test_cm_node_eq);
	cm_add_test(test_cm_node_from_word);
	cm_add_test(test_cm_node_clone);
	cm_add_test(test_cm_parse_word);
	cm_add_test(test_cm_parse_int);
	cm_add_test(test_cm_parse_expr);
	cm_add_test(test_cm_parse_compose);
	cm_add_test(test_cm_parse_extract);
	cm_add_test(test_cm_parse_transclude);
	cm_add_test(test_cm_parse_increment);
	cm_add_test(test_cm_parse_decrement);
	cm_add_test(test_cm_parse_op_invoke);
	cm_add_test(test_cm_parse_op_def);
	cm_add_test(test_cm_parse_expr_list);
	cm_add_test(test_cm_parse_match);
	cm_add_test(test_cm_parse_class);
	cm_add_test(test_cm_parse_dot);
	cm_add_test(test_cm_parse_key);
	cm_add_test(test_cm_parse_symbol_def);
	cm_add_test(test_cm_parse_relation_def);
	cm_add_test(test_cm_parse_eval);
	cm_add_test(test_cm_parse_print);
	cm_add_test(test_cm_parse_include);
	cm_add_test(test_cm_parse);
}
