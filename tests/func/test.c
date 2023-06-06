

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "func.h"
#include "../../src/file.h"


#define OUTPUT_BUFFER_SIZE 2048
#define FILENAME_BUFFER_SIZE 256


int main (int argc, char **argv)
{
	char buf_stdout[OUTPUT_BUFFER_SIZE];
	char buf_stderr[OUTPUT_BUFFER_SIZE];

	cm_shift_arg(&argc, &argv); // shift program
	char *subcommand = cm_shift_arg(&argc, &argv);

	while (argc > 0) {
		char *filename = cm_shift_arg(&argc, &argv);

		char out_filename[FILENAME_BUFFER_SIZE];
		char err_filename[FILENAME_BUFFER_SIZE];

		snprintf(out_filename, FILENAME_BUFFER_SIZE, "%s.stdout", filename);
		snprintf(err_filename, FILENAME_BUFFER_SIZE, "%s.stderr", filename);

		if (! cm_str_ends_with(filename, ".cogm")) {
			continue;
		}

		cm_run_func_test(filename, buf_stdout, buf_stderr, FILENAME_BUFFER_SIZE);

		if (strcmp(subcommand, "update") == 0) {
			cm_overwrite_file(out_filename, buf_stdout);
			cm_overwrite_file(err_filename, buf_stderr);

			printf("Writing %s stdout to %s\n", filename, out_filename);
			printf("Writing %s stderr to %s\n", filename, err_filename);

		} else if (strcmp(subcommand, "test") == 0) {
			char *expected_out = cm_read_file_to_cstr(out_filename);
			char *expected_err = cm_read_file_to_cstr(err_filename);

			bool out_failed = (strcmp(buf_stdout, expected_out) != 0);
			bool err_failed = (strcmp(buf_stdout, expected_out) != 0);

			if (! out_failed && ! err_failed) {
				printf("%s passed\n", filename);
				continue;
			}

			if (out_failed) {
				fprintf(
					stderr,
					"%s stdout failed\n\nExpected: %s\n\nActual:%s\n\n",
					filename,
					expected_out,
					buf_stdout
				);
			}

			if (err_failed) {
				fprintf(
					stderr,
					"%s stdout failed\n\nExpected: %s\n\nActual:%s\n\n",
					filename,
					expected_err,
					buf_stderr
				);
			}

		} else {
			assert(false && "Invalid subcommand");
		}
	}
}
