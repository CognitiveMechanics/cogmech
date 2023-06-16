
#include <stdio.h>
#include <stdbool.h>

#include "unit.h"
#include "../../src/context.h"
#include "test_context.h"


bool test_cm_context_create (void)
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

	CMContext cloned = cm_context_clone(&context);

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


bool test_cm_context_def_get_macro (void)
{
	CMContext context = cm_context();

	CMTokenList tokens = cm_tokenlist();
	cm_tokenlist_append(&tokens, cm_token("test.cogm", 0, 0, CM_TOKEN_TYPE_PERCENT));

	CMStringView name = cm_sv("macro");

	CMToken word = cm_token("test.cogm", 0, 0, CM_TOKEN_TYPE_WORD);
	word.value = name;

	if (cm_static_context_has_macro(&context, name)) {
		cm_test_error("Empty context has macro defined");
		return false;
	}

	cm_static_context_def_macro(&context, word, tokens);

	if (! cm_static_context_has_macro(&context, name)) {
		cm_test_error("Could not find defined macro");
		return false;
	}

	CMMacroDef retrieved = cm_static_context_get_macro(&context, name);

	if (cm_tokenlist_first(retrieved.body).type != CM_TOKEN_TYPE_PERCENT) {
		cm_test_error("Retrieved different macro than defined");
		return false;
	}

	return true;
}


void test_cm_context (void)
{
	printf("Loading interpreter tests...\n");

	cm_add_test(test_cm_context_create);
	cm_add_test(test_cm_context_clone);
	cm_add_test(test_cm_context_def_get_symbol);
	cm_add_test(test_cm_context_redef_symbol);
	cm_add_test(test_cm_context_force_def_symbol);
	cm_add_test(test_cm_context_def_get_op);
	cm_add_test(test_cm_context_def_get_relation);
}
