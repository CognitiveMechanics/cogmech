
#ifndef CM_TEST_PARSER_H
#define CM_TEST_PARSER_H

#include "../../src/parser.h"


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


bool test_cm_parse_composition (void)
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

	if (composition->type != CM_NODE_TYPE_COMPOSITION) {
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

	if (composition->children[2]->type != CM_NODE_TYPE_COMPOSITION) {
		cm_test_error("third element of composition should be CM_NODE_TYPE_COMPOSITION\n");
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

	if (composition->type != CM_NODE_TYPE_COMPOSITION) {
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


bool test_cm_parse_file (void)
{
	CMTokenList list = cm_tokenize_file("../tests/cogm/00-hello.cogm");
	CMNode *parsed = cm_parse_file(&list);

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

	if (parsed->children[1]->children[1]->type != CM_NODE_TYPE_COMPOSITION) {
		cm_test_error("right hand of second assignment should be CM_NODE_TYPE_COMPOSITION\n");
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

	cm_add_test(test_cm_node_alloc_free);
	cm_add_test(test_cm_node_symbol);
	cm_add_test(test_cm_node_literal);
	cm_add_test(test_cm_parse_expr);
	cm_add_test(test_cm_parse_composition);
	cm_add_test(test_cm_parse_symbol_def);
	cm_add_test(test_cm_parse_print);
	cm_add_test(test_cm_parse_file);
}


#endif //CM_TEST_PARSER_H
