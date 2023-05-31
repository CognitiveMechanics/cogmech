#ifndef CM_UNIT_H
#define CM_UNIT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>


typedef struct CMUnitTest {
	const char *name;
	bool (*test)();
} CMUnitTest;


#define CM_TEST_BUFFER_SIZE 2048

CMUnitTest _cm_tests[CM_TEST_BUFFER_SIZE];
size_t _cm_test_count = 0;


#define cm_test_error(...) fprintf(stderr, "FAILURE %s:%d: ", __FILE__, __LINE__); fprintf (stderr, __VA_ARGS__)
#define cm_add_test(test) assert(_cm_test_count < 2048); _cm_tests[_cm_test_count] = (CMUnitTest) {#test, &test}; _cm_test_count += 1


bool _cm_run_tests (CMUnitTest tests[], size_t n)
{
	int success_count = 0;
	int failure_count = 0;

	for (int i = 0; i < n; i += 1) {
		printf("Executing %s\n", tests[i].name);
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

bool cm_run_tests ()
{
	return _cm_run_tests(_cm_tests, _cm_test_count);
}


#endif /* CM_UNIT_H */
