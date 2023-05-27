#ifndef UNIT_H
#define UNIT_H

#include <stdbool.h>
#include <stdio.h>


typedef bool UnitTest();


bool run_tests (UnitTest *tests[], int n)
{
	int success_count = 0;
	int failure_count = 0;

	for (int i = 0; i < n; i += 1) {
		bool result = (*tests[i])();

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


#endif /* UNIT_H */
