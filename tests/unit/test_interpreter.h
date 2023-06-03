
#ifndef CM_TEST_INTERPRETER_H
#define CM_TEST_INTERPRETER_H

#include <stdio.h>

#include "unit.h"
#include "../../src/parser.h"
#include "../../src/interpreter.h"


bool test_cm_context ()
{
	CMContext context = cm_context();

	if (context.n_symbol_defs != 0) {
		cm_test_error("Empty context initialized with symbols\n");
		return false;
	}

	return true;
}


bool test_cm_context_def_get_symbol ()
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


bool test_cm_interpret_entity ()
{
	CMContext context = cm_context();

	CMStringView name = cm_sv("symbol_name");
	CMNode *symbol = cm_node_symbol(name);

	CMStringView value = cm_sv("value");
	CMNode *value_node = cm_node_literal(value);

	cm_context_def_symbol(&context, name, value_node);

	CMNode *composed = cm_node(CM_NODE_TYPE_COMPOSITION);
	cm_node_append_child(composed, symbol);

	CMNode *interpreted = cm_interpret_entity(&context, composed);

	if (interpreted->type != CM_NODE_TYPE_COMPOSITION) {
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


bool test_cm_interpret_symbol_def ()
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


bool test_cm_interpret_print ()
{
	CMContext context = cm_context();
	CMNode *op = cm_node(CM_NODE_TYPE_PRINT);
	CMStringView value = cm_sv("test output");
	CMNode *value_node = cm_node_literal(value);

	cm_node_append_child(op, value_node);
	cm_interpret_print(&context, op);

	return true;
}


bool test_cm_interpret ()
{
	CMTokenList list = cm_tokenize_file("../tests/cogm/00-hello.cogm");
	CMNode *ast = cm_parse_file(&list);
	CMContext context = cm_context();

	cm_interpret(&context, ast);

	return true;
}


void test_cm_interpreter ()
{
	printf("Loading interpreter tests...\n");

	cm_add_test(test_cm_context);
	cm_add_test(test_cm_context_def_get_symbol);
	cm_add_test(test_cm_interpret_entity);
	cm_add_test(test_cm_interpret_symbol_def);
	cm_add_test(test_cm_interpret_print);
	cm_add_test(test_cm_interpret);
}


#endif // CM_TEST_INTERPRETER_H