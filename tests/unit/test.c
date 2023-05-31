
#include "unit.h"
#include "test_file.h"
#include "test_stringview.h"
#include "test_tokenizer.h"


int main ()
{
	test_cm_stringview();
	test_cm_tokenizer();
	test_cm_file();

	return ! cm_run_tests();
}
