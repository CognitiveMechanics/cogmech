
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "file.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"

#define CM_EXIT_USAGE 1


// TODO: classes
// TODO: dot
// TODO: keys
// TODO: relations
// TODO: operations
// TODO: integers
// TODO: improve error handling
// TODO: execution test suite


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


const char *cm_shift_arg (int *argc, const char ***argv)
{
	assert((*argc) > 0);

	const char *value = (*argv)[0];

	(*argc) -= 1;
	(*argv) += 1;

	return value;
}


int main (int argc, const char **argv)
{
	cm_shift_arg(&argc, &argv);

	if (argc < 1) {
		cm_print_usage();
	}

	const char *subcommand = cm_shift_arg(&argc, &argv);
	const char *filepath = cm_shift_arg(&argc, &argv);

	if (strcmp(subcommand, "tokenize") == 0) {
		CMTokenList list = cm_tokenize_file(filepath);
		cm_print_tokenlist(list);

	} else if (strcmp(subcommand, "parse") == 0) {
		CMTokenList list = cm_tokenize_file(filepath);
		CMNode *ast = cm_parse(&list);
		cm_print_node(ast);

	} else if (strcmp(subcommand, "exec") == 0) {
		CMTokenList list = cm_tokenize_file(filepath);
		CMNode *ast = cm_parse(&list);
		CMContext context = cm_context();

		cm_interpret(&context, ast);
	}

	return 0;
}
