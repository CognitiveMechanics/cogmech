

#include <stdio.h>

#include "unit.h"
#include "test_interpreter.h"
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


bool test_cm_context_clone (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("name");
	CMNode *node = cm_node(CM_NODE_TYPE_ROOT);

	cm_context_def_symbol(&context, name, node);

	CMContext cloned = cm_context_clone(context);

	CMNode *test1 = cm_context_get_symbol(&context, name);
	CMNode *test2 = cm_context_get_symbol(&cloned, name);

	if (! cm_node_eq(test1, test2)) {
		cm_test_error("Cloned values should be equivalent");
		return false;
	}

	if (test1 == test2) {
		cm_test_error("Cloned values should not be the same");
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
		cm_test_error("Retrieved different symbol than defined");
		return false;
	}

	return true;
}


bool test_cm_context_redef_symbol (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("symbol_name");
	CMNode *node1 = cm_node(CM_NODE_TYPE_LITERAL);
	CMNode *node2 = cm_node(CM_NODE_TYPE_SYMBOL);

	cm_context_def_symbol(&context, name, node1);
	CMNode *retrieved1 = cm_context_get_symbol(&context, name);

	if (node1 != retrieved1) {
		cm_test_error("Retrieved sifferent symbol than defined");
		return false;
	}

	cm_context_redef_symbol(&context, name, node2);
	CMNode *retrieved2 = cm_context_get_symbol(&context, name);

	if (cm_node_eq(node1, retrieved2)) {
		cm_test_error("Redefined node should not be equal to original");
		return false;
	}

	if (node2 != retrieved2) {
		cm_test_error("Retrieved sifferent symbol than defined");
		return false;
	}

	return true;
}


bool test_cm_context_force_def_symbol (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("symbol_name");
	CMNode *node1 = cm_node(CM_NODE_TYPE_LITERAL);
	CMNode *node2 = cm_node(CM_NODE_TYPE_SYMBOL);

	cm_context_force_def_symbol(&context, name, node1);
	CMNode *retrieved1 = cm_context_get_symbol(&context, name);

	if (node1 != retrieved1) {
		cm_test_error("Retrieved sifferent symbol than defined");
		return false;
	}

	cm_context_force_def_symbol(&context, name, node2);
	CMNode *retrieved2 = cm_context_get_symbol(&context, name);

	if (cm_node_eq(node1, retrieved2)) {
		cm_test_error("Redefined node should not be equal to original");
		return false;
	}

	if (node2 != retrieved2) {
		cm_test_error("Retrieved different symbol than defined");
		return false;
	}

	return true;
}


bool test_cm_context_def_get_op (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("O");
	CMNode *arglist = cm_node(CM_NODE_TYPE_OP_ARGLIST);
	CMNode *body = cm_node(CM_NODE_TYPE_SYMBOL);

	if (cm_context_has_op(&context, name)) {
		cm_test_error("Empty context has op defined");
		return false;
	}

	cm_context_def_op(&context, name, arglist, body);

	if (! cm_context_has_op(&context, name)) {
		cm_test_error("Could not fine defined op");
		return false;
	}

	CMOpDef def = cm_context_get_op(&context, name);

	if (def.arglist != arglist || def.body != body || ! cm_sv_eq(def.name, name)) {
		cm_test_error("Retrieved different op than defined");
		return false;
	}

	return true;
}


bool test_cm_context_def_get_relation (void)
{
	CMContext context = cm_context();
	CMStringView bind = cm_sv("symbol_name");

	CMNode *state = cm_node(CM_NODE_TYPE_SYMBOL);
	state->value = cm_sv("test_value");

	CMNode *body = cm_node(CM_NODE_TYPE_COMPOSE);

	cm_context_def_relation(&context, bind, state, body);

	if (context.n_relation_defs != 1) {
		cm_test_error("Failed to define relation");
		return false;
	}

	CMRelationDef *def = cm_context_get_matching_relation(&context, state);

	if (! def) {
		cm_test_error("failed to retrieve matching relation");
		return false;
	}

	CMNode *no_match = cm_node(CM_NODE_TYPE_SYMBOL);
	no_match->value = cm_sv("no_match");

	CMRelationDef *nodef = cm_context_get_matching_relation(&context, no_match);

	if (nodef != NULL) {
		cm_test_error("found undefined relation");
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


bool test_cm_interpret_dot (void)
{
	CMContext context = cm_context();

	CMStringView tag = cm_sv("tag");
	CMNode *tag_node = cm_node_literal(tag);

	CMNode *proxy_node = cm_node(CM_NODE_TYPE_PROXY);

	CMNode *outer = cm_node(CM_NODE_TYPE_COMPOSE);
	CMNode *inner = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(inner, tag_node);
	cm_node_append_child(inner, proxy_node);
	cm_node_append_child(outer, inner);

	CMNode *dot = cm_node(CM_NODE_TYPE_DOT);
	cm_node_append_child(dot, outer);

	CMNode *result = cm_interpret_dot(&context, dot);

	if (outer->children[0]->children[1]->type != CM_NODE_TYPE_PROXY) {
		cm_test_error("original should still have CM_NODE_TYPE_PROXY, check that it cloned correctly");
		return false;
	}

	if (result->children[0]->children[1]->type != CM_NODE_TYPE_DOT_PROXY) {
		cm_test_error("new entity should have CM_NODE_TYPE_DOT_PROXYy");
		return false;
	}

	return true;
}


bool test_cm_interpret_increment (void)
{
	CMContext context = cm_context();

	CMNode *node1 = cm_node(CM_NODE_TYPE_INCREMENT);
	CMNode *value_node1 = cm_node_int_from_int(1);
	cm_node_append_child(node1, value_node1);

	CMNode *result1 = cm_interpret_increment(&context, node1);

	if (result1->type != CM_NODE_TYPE_INT) {
		cm_test_error("result should be type CM_NODE_TYPE_INT");
		return false;
	}

	if (cm_node_int_value(result1) != 2) {
		cm_test_error("value should be 2");
		return false;
	}

	CMNode *node2 = cm_node(CM_NODE_TYPE_INCREMENT);
	CMNode *value_node2 = cm_node_int_exact_from_int(2);
	cm_node_append_child(node2, value_node2);

	CMNode *result2 = cm_interpret_increment(&context, node2);

	if (result2->type != CM_NODE_TYPE_INT_EXACT) {
		cm_test_error("result should be type CM_NODE_TYPE_INT_EXACT");
		return false;
	}

	if (cm_node_int_value(result2) != 3) {
		cm_test_error("value should be 3");
		return false;
	}

	return true;
}


bool test_cm_interpret_decrement (void)
{
	CMContext context = cm_context();

	CMNode *node1 = cm_node(CM_NODE_TYPE_DECREMENT);
	CMNode *value_node1 = cm_node_int_from_int(1);
	cm_node_append_child(node1, value_node1);

	CMNode *result1 = cm_interpret_decrement(&context, node1);

	if (result1->type != CM_NODE_TYPE_INT) {
		cm_test_error("result should be type CM_NODE_TYPE_INT");
		return false;
	}

	if (cm_node_int_value(result1) != 0) {
		cm_test_error("value should be 0");
		return false;
	}

	CMNode *node2 = cm_node(CM_NODE_TYPE_DECREMENT);
	CMNode *value_node2 = cm_node_int_exact_from_int(2);
	cm_node_append_child(node2, value_node2);

	CMNode *result2 = cm_interpret_decrement(&context, node2);

	if (result2->type != CM_NODE_TYPE_INT_EXACT) {
		cm_test_error("result should be type CM_NODE_TYPE_INT_EXACT");
		return false;
	}

	if (cm_node_int_value(result2) != 1) {
		cm_test_error("value should be 1");
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


bool test_cm_interpret_op_def (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("O");

	CMNode *a = cm_node_symbol(cm_sv("a"));
	CMNode *end = cm_node_literal(cm_sv("end"));

	CMNode *arglist = cm_node(CM_NODE_TYPE_OP_ARGLIST);
	cm_node_append_child(arglist, a);

	CMNode *body = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(body, a);
	cm_node_append_child(body, end);

	CMNode *def_node = cm_node(CM_NODE_TYPE_OP_DEF);
	def_node->value = name;
	cm_node_append_child(def_node, arglist);
	cm_node_append_child(def_node, body);

	cm_interpret_op_def(&context, def_node);
	CMOpDef def = cm_context_get_op(&context, name);

	if (! cm_context_has_op(&context, name)) {
		cm_test_error("Could not find defined op");
		return false;
	}

	if (def.arglist != arglist || def.body != body || ! cm_sv_eq(def.name, name)) {
		cm_test_error("Retrieved different op than defined");
		return false;
	}

	return true;
}


bool test_cm_interpret_relation_def (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("a");
	CMNode *value = cm_node_literal(cm_sv("value"));

	cm_context_def_symbol(&context, name, value);

	CMNode *state = cm_node(CM_NODE_TYPE_SYMBOL);
	state->value = name;

	CMNode *body = cm_node(CM_NODE_TYPE_COMPOSE);

	CMNode *def_node = cm_node(CM_NODE_TYPE_RELATION_DEF);
	cm_node_append_child(def_node, state);
	cm_node_append_child(def_node, cm_node_null());
	cm_node_append_child(def_node, body);

	cm_interpret_relation_def(&context, def_node);

	CMRelationDef *def = cm_context_get_matching_relation(&context, value);

	if (! def) {
		cm_test_error("failed to retrieve matching relation");
		return false;
	}

	return true;
}


bool test_cm_interpret_op_invoke (void)
{
	CMContext context = cm_context();
	CMStringView name = cm_sv("O");

	CMNode *a = cm_node_symbol(cm_sv("a"));
	CMNode *start = cm_node_literal(cm_sv("start"));
	CMNode *end = cm_node_literal(cm_sv("end"));

	CMNode *arglist = cm_node(CM_NODE_TYPE_OP_ARGLIST);
	cm_node_append_child(arglist, a);

	CMNode *body = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_append_child(body, a);
	cm_node_append_child(body, end);

	cm_context_def_op(&context, name, arglist, body);

	CMNode *invoke = cm_node(CM_NODE_TYPE_OP_INVOKE);
	invoke->value = name;
	cm_node_append_child(invoke, start);

	CMNode *result = cm_interpret_op_invoke(&context, invoke);

	if (result->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("Invalid op result type");
		return false;
	}

	if (result->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("Invalid first child type");
		return false;
	}

	if (! cm_sv_eq(result->children[0]->value, cm_sv("start"))) {
		cm_test_error("Invalid first child value");
		return false;
	}

	if (result->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("Invalid second child type");
		return false;
	}

	if (! cm_sv_eq(result->children[1]->value, cm_sv("end"))) {
		cm_test_error("Invalid second child value");
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


bool test_cm_interpret_eval (void)
{
	CMContext context = cm_context();
	CMStringView bind = cm_sv("a");

	CMNode *state = cm_node(CM_NODE_TYPE_LITERAL);
	state->value = cm_sv("start");

	CMNode *body = cm_node(CM_NODE_TYPE_COMPOSE);

	CMNode *elem1 = cm_node(CM_NODE_TYPE_SYMBOL);
	elem1->value = cm_sv("a");
	cm_node_append_child(body, elem1);

	CMNode *elem2 = cm_node(CM_NODE_TYPE_LITERAL);
	elem2->value = cm_sv("end");
	cm_node_append_child(body, elem2);

	cm_context_def_relation(&context, bind, state, body);

	CMNode *eval = cm_node(CM_NODE_TYPE_EVAL);
	cm_node_append_child(eval, state);

	CMNode *result = cm_interpret_eval(&context, eval);

	if (result->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("eval returned incorrect type");
		return false;
	}

	if (result->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("eval returned incorrect first child type");
		return false;
	}

	if (! cm_sv_eq(result->children[0]->value, cm_sv("start"))) {
		cm_test_error("eval returned incorrect first child value");
		return false;
	}

	if (result->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("eval returned incorrect second child type");
		return false;
	}

	if (! cm_sv_eq(result->children[1]->value, cm_sv("end"))) {
		cm_test_error("eval returned incorrect second child");
		return false;
	}

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
		cm_tokenize_file("../tests/cogm/05-classes.cogm"),
		cm_tokenize_file("../tests/cogm/06-dot.cogm"),
		cm_tokenize_file("../tests/cogm/07-keys.cogm"),
		cm_tokenize_file("../tests/cogm/08-relations.cogm"),
		cm_tokenize_file("../tests/cogm/09-operations.cogm"),
		cm_tokenize_file("../tests/cogm/10-integers.cogm"),
		cm_tokenize_file("../tests/cogm/11-increment-decrement.cogm"),
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
	cm_add_test(test_cm_context_clone);
	cm_add_test(test_cm_context_def_get_symbol);
	cm_add_test(test_cm_context_redef_symbol);
	cm_add_test(test_cm_context_force_def_symbol);
	cm_add_test(test_cm_context_def_get_op);
	cm_add_test(test_cm_context_def_get_relation);
	cm_add_test(test_cm_create_key_value);
	cm_add_test(test_cm_interpret_entity);
	cm_add_test(test_cm_interpret_extract);
	cm_add_test(test_cm_interpret_transclude);
	cm_add_test(test_cm_interpret_eval);
	cm_add_test(test_cm_interpret_increment);
	cm_add_test(test_cm_interpret_decrement);
	cm_add_test(test_cm_interpret_match);
	cm_add_test(test_cm_interpret_dot);
	cm_add_test(test_cm_match);
	cm_add_test(test_cm_interpret_symbol_def);
	cm_add_test(test_cm_interpret_relation_def);
	cm_add_test(test_cm_interpret_op_def);
	cm_add_test(test_cm_interpret_op_invoke);
	cm_add_test(test_cm_interpret_print);
	cm_add_test(test_cm_interpret);
}

