
#include "unit.h"
#include "test_tokenize.c"


int main ()
{
	cm_test_tokenize();

	return ! cm_run_tests();
}
