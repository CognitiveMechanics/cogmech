
#include <stdbool.h>
#include <stdio.h>

#include "unit.h"


CMUnitTest _cm_tests[CM_TEST_BUFFER_SIZE];
size_t _cm_test_count = 0;


bool _cm_run_tests (CMUnitTest tests[], size_t n)
{
	int success_count = 0;
	int failure_count = 0;

	for (size_t i = 0; i < n; i += 1) {
		printf("Executing %s...\n", tests[i].name);
		bool result = (* (tests[i].test))();

		if (!result) {
			failure_count += 1;
			continue;
		}

		success_count += 1;
	}

	if (failure_count) {
		fprintf(stderr, "TESTS FAILED: %d successes and %d failures\n", success_count, failure_count);
		return false;
	}

	printf("TESTS SUCCEEDED: %d successes and %d failures\n", success_count, failure_count);
	return true;
}


bool cm_run_tests (void)
{
	return _cm_run_tests(_cm_tests, _cm_test_count);
}

