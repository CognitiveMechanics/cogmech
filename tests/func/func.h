
#ifndef CM_FUNC_H
#define CM_FUNC_H

#define CM_FUNC_TEST_BUFFER_SIZE 10240
#define CM_FILENAME_BUFFER_SIZE 256

#include <stdlib.h>
#include <stdbool.h>


static char *CM_FUNC_TEST_FILES[] = {
	"../cogm/tests/00-hello.cogm",
	"../cogm/tests/01-silent.cogm",
	"../cogm/tests/02-extract.cogm",
	"../cogm/tests/03-transclude.cogm",
	"../cogm/tests/04-match.cogm",
	"../cogm/tests/05-classes.cogm",
	"../cogm/tests/06-dot.cogm",
	"../cogm/tests/07-keys.cogm",
	"../cogm/tests/08-relations.cogm",
	"../cogm/tests/09-operations.cogm",
	"../cogm/tests/10-integers.cogm",
	"../cogm/tests/11-increment-decrement.cogm",
	"../cogm/tests/12-include.cogm",
	"../cogm/tests/13-math.cogm",
	"../cogm/tests/14-comments.cogm",
	"../cogm/tests/15-macros.cogm",
	"../cogm/tests/16-test-trace-1.cogm",
	"../cogm/tests/17-test-trace-2.cogm",

	"../cogm/universal/tests/compose.cogm",
	"../cogm/universal/tests/extract.cogm",
	"../cogm/universal/tests/incr-decr.cogm",
	"../cogm/universal/tests/ref.cogm",
	"../cogm/universal/tests/transclude.cogm",
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
	"../cogm/errors/25-unterminated-statement.cogm",
	"../cogm/errors/26-macro-error-location.cogm",
	"../cogm/errors/27-macro-redefined.cogm",
};


void cm_run_func_test (char *filename, char *buf_stdout, char *buf_stderr, size_t buf_size);
bool cm_run_func_tests (size_t n_files, char **filenames);
void cm_update_func_tests (size_t n_files, char **filenames);


#endif // CM_FUNC_H
