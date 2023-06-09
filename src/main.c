
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "file.h"
#include "util.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"

#define CM_EXIT_USAGE 1


// TODO: implement universal machine
// TODO: add documentation


const char* cm_get_usage (void)
{
	return
		"\n"
		"OVERVIEW: A utility for working with cogmech files\n"
		"\n"
		"USAGE: cogm <subcommand> [args]\n"
		"\n"
		"SUBCOMMANDS:\n"
		"  tokenize <file>       Tokenizes a valid .cogm <file>\n"
		"  parse <file>          Parses a valid .cogm <file> into an AST\n"
		"  exec <file>           Parses and executes a valid .cogm <file>\n";
}


void cm_print_usage (void)
{
	const char *usage = cm_get_usage();
	printf("%s\n", usage);
	exit(CM_EXIT_USAGE);
}


int main (int argc, char **argv)
{
	cm_shift_arg(&argc, &argv);

	if (argc < 1) {
		cm_print_usage();
	}

	const char *subcommand = cm_shift_arg(&argc, &argv);

	if (argc < 1) {
		cm_print_usage();
	}

	const char *filepath = cm_shift_arg(&argc, &argv);
	CMContext context = cm_context();

	if (strcmp(subcommand, "tokenize") == 0) {
		CMTokenList list = cm_tokenize_file(&context, filepath);
		cm_print_tokenlist(list);

	} else if (strcmp(subcommand, "parse") == 0) {
		CMTokenList list = cm_tokenize_file(&context, filepath);
		CMNode *ast = cm_parse(&list);
		cm_print_node(ast);

	} else if (strcmp(subcommand, "exec") == 0) {
		CMTokenList list = cm_tokenize_file(&context, filepath);
		CMNode *ast = cm_parse(&list);
		cm_interpret(&context, ast);

	} else {
		cm_print_usage();
	}

	return 0;
}
