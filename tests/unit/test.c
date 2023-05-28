
#include "unit.h"
#include "test_stringview.c"


int main ()
{
	test_cm_stringview();

	return ! cm_run_tests();
}
