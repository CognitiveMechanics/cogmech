#ifndef CM_UNIT_H
#define CM_UNIT_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


typedef struct CMUnitTest {
	const char *name;
	bool (*test)(void);
} CMUnitTest;


#define CM_TEST_BUFFER_SIZE 2048

extern CMUnitTest _cm_tests[CM_TEST_BUFFER_SIZE];
extern size_t _cm_test_count;


#define cm_test_error(...) fprintf(stderr, "FAILURE %s:%d: ", __FILE__, __LINE__); fprintf (stderr, __VA_ARGS__); fprintf(stderr, "\n")
#define cm_add_test(test) assert(_cm_test_count < CM_TEST_BUFFER_SIZE); _cm_tests[_cm_test_count] = (CMUnitTest) {#test, &test}; _cm_test_count += 1


bool cm_run_tests (void);


#endif /* CM_UNIT_H */
