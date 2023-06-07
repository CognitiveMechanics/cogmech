
#include "unit.h"
#include "test_func.h"
#include "../../src/util.h"

#include <stdbool.h>


bool test_cm_func_tests (void)
{
	bool result = cm_run_func_tests(ARRAY_LEN(CM_FUNC_TEST_FILES), CM_FUNC_TEST_FILES);

	if (! result) {
		cm_test_error("Func tests failed");
		return false;
	}

	return true;
}


bool test_cm_func_error_tests (void)
{
	bool result = cm_run_func_tests(ARRAY_LEN(CM_FUNC_TEST_ERROR_FILES), CM_FUNC_TEST_ERROR_FILES);

	if (! result) {
		cm_test_error("Func tests failed");
		return false;
	}

	return true;
}


void test_cm_func (void)
{
	cm_add_test(test_cm_func_tests);
	cm_add_test(test_cm_func_error_tests);
}
