
#ifndef CM_TEST_INTERPRETER_H
#define CM_TEST_INTERPRETER_H

#include <stdio.h>

#include "unit.h"
#include "../../src/parser.h"
#include "../../src/interpreter.h"


bool test_cm_context (void)
{
	CMContext context = cm_context();

	if (context.n_symbol_defs != 0) {
		cm_test_error("Empty context initialized with symbols\n");
		return false;
	}

	return true;
}


bool test_cm_context_def_get_symbol (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("symbol_name");
	CMNode *node = cm_node(CM_NODE_TYPE_LITERAL);

	if (cm_context_has_symbol(&context, name)) {
		cm_test_error("Empty context has symbol defined");
		return false;
	}

	cm_context_def_symbol(&context, name, node);

	if (! cm_context_has_symbol(&context, name)) {
		cm_test_error("Could not fine defined symbol");
		return false;
	}

	CMNode *retrieved = cm_context_get_symbol(&context, name);

	if (node != retrieved) {
		cm_test_error("Retrieved sifferent symbol than defined");
		return false;
	}

	return true;
}


bool test_cm_create_key_value (void)
{
	CMStringView key = cm_sv("key");
	CMNode *key_node = cm_node_literal(key);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	CMNode *node = cm_create_key_value(key_node, value_node);

	if (node->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("key node should be of type CM_NODE_TYPE_LITERAL");
		return false;
	}

	if (! cm_sv_eq(node->children[0]->value, cm_sv("key"))) {
		cm_test_error("Invalid value of key node");
		return false;
	}

	if (node->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("value node should be of type CM_NODE_TYPE_LITERAL");
		return false;
	}

	if (! cm_sv_eq(node->children[1]->value, cm_sv("value"))) {
		cm_test_error("Invalid value of value node");
		return false;
	}

	return true;
}


bool test_cm_interpret_entity (void)
{
	CMContext context = cm_context();

	CMStringView name = cm_sv("symbol_name");
	CMNode *symbol = cm_node_symbol(name);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	cm_context_def_symbol(&context, name, value_node);

	CMNode *composed = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(composed, symbol);

	CMNode *interpreted = cm_interpret_entity(&context, composed);

	if (interpreted->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("Invalid root node type");
		return false;
	}

	if (interpreted->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("Invalid child node type");
		return false;
	}

	if (! cm_sv_eq(interpreted->children[0]->value, value)) {
		cm_test_error("invalid interpretted value");
		return false;
	}

	return true;
}


bool test_cm_interpret_symbol_def (void)
{
	CMContext context = cm_context();

	CMStringView name = cm_sv("symbol_name");
	CMNode *symbol_node = cm_node_symbol(name);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	CMNode *def = cm_node(CM_NODE_TYPE_SYMBOL_DEF);
	cm_node_append_child(def, symbol_node);
	cm_node_append_child(def, value_node);

	cm_interpret_symbol_def(&context, def);
	CMNode *stored = cm_context_get_symbol(&context, name);

	if (stored == value_node) {
		cm_test_error("invalid stored value");
		return false;
	}

	return true;
}


bool test_cm_interpret_extract (void)
{
	CMContext context = cm_context();

	CMStringView name = cm_sv("symbol_name");
	CMNode *symbol_node = cm_node_symbol(name);

	CMStringView tag = cm_sv("tag");
	CMNode *tag_node = cm_node_literal(tag);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	CMNode *outer = cm_node(CM_NODE_TYPE_COMPOSE);
	CMNode *inner = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(inner, tag_node);
	cm_node_append_child(inner, value_node);
	cm_node_append_child(outer, inner);

	cm_context_def_symbol(&context, name, outer);

	CMNode *extraction = cm_node(CM_NODE_TYPE_EXTRACT);
	cm_node_append_child(extraction, symbol_node);
	cm_node_append_child(extraction, tag_node);

	CMNode *result = cm_interpret_extract(&context, extraction);

	if (! cm_node_eq(result, value_node)) {
		cm_test_error("invalid extracted value");
		return false;
	}

	return true;
}


bool test_cm_interpret_transclude (void)
{
	CMContext context = cm_context();

	CMStringView name = cm_sv("symbol_name");
	CMNode *symbol_node = cm_node_symbol(name);

	CMStringView tag = cm_sv("tag");
	CMNode *tag_node = cm_node_literal(tag);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	CMStringView new_value = cm_sv("new_value");
	CMNode *new_value_node = cm_node_literal(new_value);

	CMNode *outer = cm_node(CM_NODE_TYPE_COMPOSE);
	CMNode *inner = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(inner, tag_node);
	cm_node_append_child(inner, value_node);
	cm_node_append_child(outer, inner);

	cm_context_def_symbol(&context, name, outer);

	CMNode *transclusion = cm_node(CM_NODE_TYPE_TRANSCLUDE);
	cm_node_append_child(transclusion, symbol_node);
	cm_node_append_child(transclusion, tag_node);
	cm_node_append_child(transclusion, new_value_node);

	CMNode *transcluded = cm_interpret_transclude(&context, transclusion);

	CMNode *extraction = cm_node(CM_NODE_TYPE_EXTRACT);
	cm_node_append_child(extraction, transcluded);
	cm_node_append_child(extraction, tag_node);

	CMNode *result = cm_interpret_extract(&context, extraction);

	if (! cm_node_eq(result, new_value_node)) {
		cm_test_error("invalid extracted value");
		return false;
	}

	return true;
}


bool test_cm_interpret_match (void)
{
	CMContext context = cm_context();

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

	CMNode *true_match = cm_node(CM_NODE_TYPE_MATCH);
	cm_node_append_child(true_match, outer_1);
	cm_node_append_child(true_match, outer_2);

	CMNode *false_match = cm_node(CM_NODE_TYPE_MATCH);
	cm_node_append_child(false_match, outer_2);
	cm_node_append_child(false_match, outer_1);

	CMNode *true_result = cm_interpret_match(&context, true_match);
	CMNode *false_result = cm_interpret_match(&context, false_match);

	if (! cm_node_eq(true_result, cm_node(CM_NODE_TYPE_TRUE))) {
		cm_test_error("true result is not true");
		return false;
	}

	if (! cm_node_eq(false_result, cm_node_null())) {
		cm_test_error("false result is not null");
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

	return true;
}


bool test_cm_interpret_print (void)
{
	CMContext context = cm_context();
	CMNode *op = cm_node(CM_NODE_TYPE_PRINT);
	CMStringView value = cm_sv("test output");
	CMNode *value_node = cm_node_literal(value);

	cm_node_append_child(op, value_node);
	cm_interpret_print(&context, op);

	return true;
}


bool test_cm_interpret (void)
{
	CMTokenList lists[] = {
		cm_tokenize_file("../tests/cogm/00-hello.cogm"),
		cm_tokenize_file("../tests/cogm/01-silent.cogm"),
		cm_tokenize_file("../tests/cogm/02-extract.cogm"),
		cm_tokenize_file("../tests/cogm/03-transclude.cogm"),
		cm_tokenize_file("../tests/cogm/04-match.cogm"),
	};

	for (size_t i = 0; i < ARRAY_LEN(lists); i++) {
		CMNode *ast = cm_parse(&lists[i]);
		CMContext context = cm_context();
		cm_interpret(&context, ast);
	}

	return true;
}


void test_cm_interpreter (void)
{
	printf("Loading interpreter tests...\n");

	cm_add_test(test_cm_context);
	cm_add_test(test_cm_context_def_get_symbol);
	cm_add_test(test_cm_create_key_value);
	cm_add_test(test_cm_interpret_entity);
	cm_add_test(test_cm_interpret_extract);
	cm_add_test(test_cm_interpret_transclude);
	cm_add_test(test_cm_interpret_match);
	cm_add_test(test_cm_match);
	cm_add_test(test_cm_interpret_symbol_def);
	cm_add_test(test_cm_interpret_print);
	cm_add_test(test_cm_interpret);
}


#endif // CM_TEST_INTERPRETER_H
