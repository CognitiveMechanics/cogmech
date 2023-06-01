
#ifndef CM_TEST_PARSER_H
#define CM_TEST_PARSER_H

#include "../../src/parser.h"


bool test_cm_node_alloc_free ()
{
	CMNode *parent = cm_node(CM_NODE_TYPE_ROOT);

	if (parent->children == NULL) {
		cm_test_error("test_cm_node_alloc_free_children: failed to allocate parent children\n");
		return false;
	}

	if (parent->n_children != 0) {
		cm_test_error("test_cm_node_alloc_free_children: parent node indicates children before any have been appended\n");
		return false;
	}
	
	CMNode *child = cm_node(CM_NODE_TYPE_SYMBOL);

	if (child->children == NULL) {
		cm_test_error("test_cm_node_alloc_free_children: failed to allocate child children\n");
		return false;
	}

	if (child->n_children != 0) {
		cm_test_error("test_cm_node_alloc_free_children: child node indicates children before any have been appended\n");
		return false;
	}
	
	CMNode *grandchild = cm_node(CM_NODE_TYPE_LITERAL);

	cm_node_append_child(child, grandchild);
	cm_node_append_child(parent, child);

	if (child->n_children != 1) {
		cm_test_error("test_cm_node_alloc_free_children: child node indicates no children\n");
		return false;
	}

	if (parent->n_children != 1) {
		cm_test_error("test_cm_node_alloc_free_children: parent node indicates no children\n");
		return false;
	}

	cm_node_free(parent);

	if (child->n_children != 0) {
		cm_test_error("test_cm_node_alloc_free_children: child still indicates children after parent has been freed\n");
		return false;
	}

	if (parent->n_children != 0) {
		cm_test_error("test_cm_node_alloc_free_children: parent still indicates children after it has been freed\n");
		return false;
	}
	
	return true;
}


void test_cm_parser ()
{
	printf("Loading parser tests...\n");

	cm_add_test(test_cm_node_alloc_free);
}


#endif //CM_TEST_PARSER_H