
#include "unit.h"
#include "test_stringview.c"


int main ()
{
	cm_test_stringview();

	return ! cm_run_tests();
}
