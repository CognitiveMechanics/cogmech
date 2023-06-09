

#include "unit.h"
#include "test_node.h"
#include "../../src/tokenlist.h"
#include "../../src/node.h"
#include "../../src/tokenizer.h"
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
	CMContext context = cm_context();

	CMTokenList seq1 = cm_tokenize(&context, "test1.cogm", cm_sv(" a  := <\"b\", c>\n"));
	CMTokenList seq2 = cm_tokenize(&context, "test2.cogm", cm_sv("a := < \"b\" , c > \n"));
	CMTokenList seq3 = cm_tokenize(&context, "test3.cogm", cm_sv("a := <\"b\", x>\n"));

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
	CMContext context = cm_context();
	CMTokenList seq1 = cm_tokenize(&context, "test1.cogm", cm_sv(" a  := <\"b\", c>\n"));

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


bool test_cm_match (void)
{
	CMStringView tag = cm_sv("tag");
	CMNode *tag_node = cm_node_literal(tag);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	CMNode *proxy_node = cm_node(CM_NODE_TYPE_PROXY);

	CMNode *outer_1 = cm_node(CM_NODE_TYPE_COMPOSE);
	CMNode *inner_1 = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(inner_1, tag_node);
	cm_node_append_child(inner_1, value_node);
	cm_node_append_child(outer_1, inner_1);

	CMNode *outer_2 = cm_node(CM_NODE_TYPE_COMPOSE);
	CMNode *inner_2 = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(inner_2, tag_node);
	cm_node_append_child(inner_2, proxy_node);
	cm_node_append_child(outer_2, inner_2);

	bool true_result = cm_match(outer_1, outer_2);
	bool false_result = cm_match(outer_2, outer_1);

	if (! true_result) {
		cm_test_error("true result is not true");
		return false;
	}

	if (false_result) {
		cm_test_error("false result is not false");
		return false;
	}

	CMNode *int_0 = cm_node_int(cm_sv("0"));
	CMNode *int_1 = cm_node_int(cm_sv("1"));
	CMNode *int_2 = cm_node_int(cm_sv("2"));

	CMNode *int_e_0 = cm_node_int_exact(cm_sv("0"));
	CMNode *int_e_1 = cm_node_int_exact(cm_sv("1"));
	CMNode *int_e_2 = cm_node_int_exact(cm_sv("2"));

	if (! cm_match(int_1, int_0)) {
		cm_test_error("1 should match 0");
		return false;
	}

	if (! cm_match(int_1, int_1)) {
		cm_test_error("1 should match 1");
		return false;
	}

	if (cm_match(int_1, int_2)) {
		cm_test_error("1 should not match 2");
		return false;
	}

	if (cm_match(int_1, int_e_0)) {
		cm_test_error("1 should not match exactly 0");
		return false;
	}

	if (! cm_match(int_1, int_e_1)) {
		cm_test_error("1 should match exactly 1");
		return false;
	}

	if (cm_match(int_1, int_e_2)) {
		cm_test_error("1 should not match exactly 2");
		return false;
	}

	return true;
}


void test_cm_node (void)
{
	printf("Loading node tests...\n");

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
	cm_add_test(test_cm_match);
}
