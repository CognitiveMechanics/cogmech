
#ifndef CM_TEST_INTERPRETER_H
#define CM_TEST_INTERPRETER_H

#include <stdio.h>

#include "unit.h"
#include "../../src/parser.h"
#include "../../src/interpreter.h"


bool test_cm_interpret ()
{
	CMTokenList list = cm_tokenize_file("../tests/cogm/01-silent.cogm");
	CMNode *ast = cm_parse_file(&list);
	CMContext context = {0};

	cm_interpret(&context, ast);

	return true;
}


void test_cm_interpreter ()
{
	printf("Loading interpreter tests...\n");

	cm_add_test(test_cm_interpret);
}


#endif // CM_TEST_INTERPRETER_H