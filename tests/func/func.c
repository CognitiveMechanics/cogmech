
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "func.h"


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


char *cm_shift_arg (int *argc, char ***argv)
{
	assert((*argc) > 0);

	char *value = (*argv)[0];

	(*argc) -= 1;
	(*argv) += 1;

	return value;
}


void cm_overwrite_file (const char* filename, char* content) {
	FILE* file = fopen(filename, "w");

	assert(file != NULL && "Failed to open the file.\n");
	fputs(content, file);

	fclose(file);
}


bool cm_str_ends_with(const char* str, const char* suffix) {
	size_t str_length = strlen(str);
	size_t suffix_length = strlen(suffix);

	if (suffix_length > str_length) {
		return false;
	}

	const char* str_end = str + (str_length - suffix_length);

	return strcmp(str_end, suffix) == 0;
}