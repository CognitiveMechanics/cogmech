
#include "unit.h"
#include "test_file.h"
#include "test_stringview.h"
#include "test_tokenizer.h"
#include "test_parser.h"


int main ()
{
	test_cm_file();
	test_cm_stringview();
	test_cm_tokenizer();
	test_cm_parser();

	return ! cm_run_tests();
}
