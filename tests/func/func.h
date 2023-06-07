
#ifndef CM_FUNC_H
#define CM_FUNC_H

#define CM_FUNC_TEST_BUFFER_SIZE 2048
#define CM_FILENAME_BUFFER_SIZE 256

#include <stdlib.h>
#include <stdbool.h>


static char *CM_FUNC_TEST_FILES[] = {
	"../tests/cogm/00-hello.cogm",
	"../tests/cogm/01-silent.cogm",
	"../tests/cogm/02-extract.cogm",
	"../tests/cogm/03-transclude.cogm",
	"../tests/cogm/04-match.cogm",
	"../tests/cogm/05-classes.cogm",
	"../tests/cogm/06-dot.cogm",
	"../tests/cogm/07-keys.cogm",
	"../tests/cogm/08-relations.cogm",
	"../tests/cogm/09-operations.cogm",
	"../tests/cogm/10-integers.cogm",
	"../tests/cogm/11-increment-decrement.cogm",
};


void cm_run_func_test (char *filename, char *buf_stdout, char *buf_stderr, size_t buf_size);
bool cm_run_func_tests (size_t n_files, char **filenames);
void cm_update_func_tests (size_t n_files, char **filenames);


#endif // CM_FUNC_H
