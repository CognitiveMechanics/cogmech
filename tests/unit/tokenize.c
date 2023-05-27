#include "unit.h"
#include "../../src/tokenize.h"
#include <stdio.h>
#include <stdbool.h>


bool always_true ()
{
	return true;
}

bool always_fails ()
{
	return false;
}


int main ()
{
	UnitTest *tests[] = {
        always_true,
        always_fails
    };

    return run_tests(tests, sizeof(tests) / sizeof(UnitTest *));
}
