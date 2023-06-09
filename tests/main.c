
#include "unit/unit.h"
#include "unit/test_file.h"
#include "unit/test_func.h"
#include "unit/test_stringview.h"
#include "unit/test_token.h"
#include "unit/test_tokenlist.h"
#include "unit/test_tokenizer.h"
#include "unit/test_node.h"
#include "unit/test_parser.h"
#include "unit/test_context.h"
#include "unit/test_interpreter.h"


int main (void)
{
	test_cm_file();
	test_cm_stringview();
	test_cm_token();
	test_cm_tokenlist();
	test_cm_tokenizer();
	test_cm_node();
	test_cm_parser();
	test_cm_context();
	test_cm_interpreter();
	test_cm_func();

	return ! cm_run_tests();
}
