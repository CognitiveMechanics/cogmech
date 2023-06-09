
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "func.h"
#include "../../src/file.h"
#include "../../src/util.h"


void cm_run_func_test (char *filename, char *buf_stdout, char *buf_stderr, size_t buf_size) {
	int pipe_stdout[2]; // Pipe for child's stdout
	int pipe_stderr[2]; // Pipe for child's stderr

	// Create the pipes
	if (pipe(pipe_stdout) == -1 || pipe(pipe_stderr) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	// Fork the process
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		// Child process

		// Close unnecessary pipe ends
		close(pipe_stdout[0]);
		close(pipe_stderr[0]);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		// Duplicate pipe ends to stdout and stderr
		dup2(pipe_stdout[1], STDOUT_FILENO);
		dup2(pipe_stderr[1], STDERR_FILENO);

		// Replace the child process with the desired command
		execvp("./cogmech", (char *[]){
			"./cogmech",
			"exec",
			filename,
			NULL
		});

		// If exec fails, it will reach this point
		perror("exec");
		exit(EXIT_FAILURE);
	} else {
		// Parent process

		// Close unused pipe ends
		close(pipe_stdout[1]);
		close(pipe_stderr[1]);

		ssize_t stdout_bytes;
		buf_stdout[0] = '\0';

		while ((stdout_bytes = read(pipe_stdout[0], buf_stdout, buf_size - 1)) != 0) {
			if (stdout_bytes == -1) {
				perror("read stdout");
				exit(EXIT_FAILURE);
			}
			buf_stdout[stdout_bytes] = '\0';
		}

		ssize_t stderr_bytes;
		buf_stderr[0] = '\0';

		while ((stderr_bytes = read(pipe_stderr[0], buf_stderr, buf_size - 1)) != 0) {
			if (stderr_bytes == -1) {
				perror("read stderr");
				exit(EXIT_FAILURE);
			}
			buf_stderr[stderr_bytes] = '\0';
		}
	}
}


bool cm_run_func_tests(size_t n_files, char **filenames)
{
	bool success = true;

	char buf_stdout[CM_FUNC_TEST_BUFFER_SIZE];
	char buf_stderr[CM_FUNC_TEST_BUFFER_SIZE];

	for (size_t i = 0; i < n_files; i++) {
		char *filename = filenames[i];char out_filename[CM_FILENAME_BUFFER_SIZE];
		char err_filename[CM_FILENAME_BUFFER_SIZE];

		snprintf(out_filename, CM_FILENAME_BUFFER_SIZE, "%s.stdout", filename);
		snprintf(err_filename, CM_FILENAME_BUFFER_SIZE, "%s.stderr", filename);

		if (! cm_str_ends_with(filename, ".cogm")) {
			continue;
		}

		cm_run_func_test(filename, buf_stdout, buf_stderr, CM_FILENAME_BUFFER_SIZE);

		char *expected_out = cm_read_file_to_cstr(out_filename);
		char *expected_err = cm_read_file_to_cstr(err_filename);

		bool out_failed = (strcmp(buf_stdout, expected_out) != 0);
		bool err_failed = (strcmp(buf_stderr, expected_err) != 0);

		if (! out_failed && ! err_failed) {
			printf("%s passed\n", filename);
			continue;
		}

		success = false;

		if (out_failed) {
			fprintf(
				stderr,
				"%s stdout failed\nExpected : %s\nActual   : %s\n",
				filename,
				expected_out,
				buf_stdout
			);
		}

		if (err_failed) {
			fprintf(
				stderr,
				"%s stderr failed\nExpected : %s\nActual   : %s\n",
				filename,
				expected_err,
				buf_stderr
			);
		}
	}

	return success;
}


void cm_update_func_tests(size_t n_files, char **filenames)
{
	char buf_stdout[CM_FUNC_TEST_BUFFER_SIZE];
	char buf_stderr[CM_FUNC_TEST_BUFFER_SIZE];

	for (size_t i = 0; i < n_files; i++) {
		char *filename = filenames[i];char out_filename[CM_FILENAME_BUFFER_SIZE];
		char err_filename[CM_FILENAME_BUFFER_SIZE];

		snprintf(out_filename, CM_FILENAME_BUFFER_SIZE, "%s.stdout", filename);
		snprintf(err_filename, CM_FILENAME_BUFFER_SIZE, "%s.stderr", filename);

		if (! cm_str_ends_with(filename, ".cogm")) {
			continue;
		}

		cm_run_func_test(filename, buf_stdout, buf_stderr, CM_FILENAME_BUFFER_SIZE);

		printf("Writing %s stdout to %s:\n%s\n", filename, out_filename, buf_stdout);
		printf("Writing %s stderr to %s:\n%s\n", filename, err_filename, buf_stderr);

		cm_overwrite_file(out_filename, buf_stdout);
		cm_overwrite_file(err_filename, buf_stderr);
	}
}
