
#include "unit.h"
#include "test_file.c"
#include "test_stringview.c"
#include "test_tokenizer.c"


int main ()
{
	test_cm_stringview();
	test_cm_tokenizer();
	test_cm_file();

	return ! cm_run_tests();
}
