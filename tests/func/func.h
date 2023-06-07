
#ifndef CM_FUNC_H
#define CM_FUNC_H

#define CM_FUNC_TEST_BUFFER_SIZE 2048
#define CM_FILENAME_BUFFER_SIZE 256

#include <stdlib.h>
#include <stdbool.h>


static char *CM_FUNC_TEST_FILES[] = {
	"../tests/cogm/examples/00-hello.cogm",
	"../tests/cogm/examples/01-silent.cogm",
	"../tests/cogm/examples/02-extract.cogm",
	"../tests/cogm/examples/03-transclude.cogm",
	"../tests/cogm/examples/04-match.cogm",
	"../tests/cogm/examples/05-classes.cogm",
	"../tests/cogm/examples/06-dot.cogm",
	"../tests/cogm/examples/07-keys.cogm",
	"../tests/cogm/examples/08-relations.cogm",
	"../tests/cogm/examples/09-operations.cogm",
	"../tests/cogm/examples/10-integers.cogm",
	"../tests/cogm/examples/11-increment-decrement.cogm",
};


static char *CM_FUNC_TEST_ERROR_FILES[] = {
	"../tests/cogm/errors/00-unterminated-quote.cogm",
	"../tests/cogm/errors/01-invalid-token.cogm",
	"../tests/cogm/errors/02-transclude-invalid-args.cogm",
	"../tests/cogm/errors/03-transclude-unstructured.cogm",
	"../tests/cogm/errors/04-match-invalid-args.cogm",
	"../tests/cogm/errors/05-invalid-dot.cogm",
	"../tests/cogm/errors/06-invalid-begin-statement.cogm",
	"../tests/cogm/errors/07-expected-expression.cogm",
	"../tests/cogm/errors/08-invalid-op-definition-param.cogm",
	"../tests/cogm/errors/09-word-statement-must-define.cogm",
	"../tests/cogm/errors/10-undefined-op.cogm",
	"../tests/cogm/errors/11-op-invalid-args.cogm",
	"../tests/cogm/errors/12-undefined-symbol.cogm",
	"../tests/cogm/errors/13-redefined-symbol.cogm",
	"../tests/cogm/errors/14-redefined-op.cogm",
	"../tests/cogm/errors/15-class-closing-bracket.cogm",
	"../tests/cogm/errors/16-unterminated-expr-list.cogm",
	"../tests/cogm/errors/17-invalid-eval.cogm",
	"../tests/cogm/errors/18-invalid-increment.cogm",
	"../tests/cogm/errors/19-invalid-increment-args.cogm",
	"../tests/cogm/errors/20-invalid-decrement.cogm",
	"../tests/cogm/errors/21-invalid-decrement-args.cogm",
	"../tests/cogm/errors/22-invalid-key.cogm",
	"../tests/cogm/errors/23-invalid-op-def.cogm",
	"../tests/cogm/errors/24-end-of-tokens.cogm",
};


void cm_run_func_test (char *filename, char *buf_stdout, char *buf_stderr, size_t buf_size);
bool cm_run_func_tests (size_t n_files, char **filenames);
void cm_update_func_tests (size_t n_files, char **filenames);


#endif // CM_FUNC_H
