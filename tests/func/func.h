
#ifndef CM_FUNC_H
#define CM_FUNC_H

#define CM_FUNC_TEST_BUFFER_SIZE 2048
#define CM_FILENAME_BUFFER_SIZE 256

#include <stdlib.h>
#include <stdbool.h>


static char *CM_FUNC_TEST_FILES[] = {
	"../cogm/examples/00-hello.cogm",
	"../cogm/examples/01-silent.cogm",
	"../cogm/examples/02-extract.cogm",
	"../cogm/examples/03-transclude.cogm",
	"../cogm/examples/04-match.cogm",
	"../cogm/examples/05-classes.cogm",
	"../cogm/examples/06-dot.cogm",
	"../cogm/examples/07-keys.cogm",
	"../cogm/examples/08-relations.cogm",
	"../cogm/examples/09-operations.cogm",
	"../cogm/examples/10-integers.cogm",
	"../cogm/examples/11-increment-decrement.cogm",
	"../cogm/examples/12-include.cogm",
};


static char *CM_FUNC_TEST_ERROR_FILES[] = {
	"../cogm/errors/00-unterminated-quote.cogm",
	"../cogm/errors/01-invalid-token.cogm",
	"../cogm/errors/02-transclude-invalid-args.cogm",
	"../cogm/errors/03-transclude-unstructured.cogm",
	"../cogm/errors/04-match-invalid-args.cogm",
	"../cogm/errors/05-invalid-dot.cogm",
	"../cogm/errors/06-invalid-begin-statement.cogm",
	"../cogm/errors/07-expected-expression.cogm",
	"../cogm/errors/08-invalid-op-definition-param.cogm",
	"../cogm/errors/09-word-statement-must-define.cogm",
	"../cogm/errors/10-undefined-op.cogm",
	"../cogm/errors/11-op-invalid-args.cogm",
	"../cogm/errors/12-undefined-symbol.cogm",
	"../cogm/errors/13-redefined-symbol.cogm",
	"../cogm/errors/14-redefined-op.cogm",
	"../cogm/errors/15-class-closing-bracket.cogm",
	"../cogm/errors/16-unterminated-expr-list.cogm",
	"../cogm/errors/17-invalid-eval.cogm",
	"../cogm/errors/18-invalid-increment.cogm",
	"../cogm/errors/19-invalid-increment-args.cogm",
	"../cogm/errors/20-invalid-decrement.cogm",
	"../cogm/errors/21-invalid-decrement-args.cogm",
	"../cogm/errors/22-invalid-key.cogm",
	"../cogm/errors/23-invalid-op-def.cogm",
	"../cogm/errors/24-end-of-tokens.cogm",
};


void cm_run_func_test (char *filename, char *buf_stdout, char *buf_stderr, size_t buf_size);
bool cm_run_func_tests (size_t n_files, char **filenames);
void cm_update_func_tests (size_t n_files, char **filenames);


#endif // CM_FUNC_H
