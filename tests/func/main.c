
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "func.h"
#include "../../src/util.h"


int main (int argc, char **argv)
{
	cm_shift_arg(&argc, &argv); // shift program
	char *subcommand = cm_shift_arg(&argc, &argv);

	if (strcmp(subcommand, "update") == 0) {
		cm_update_func_tests(argc, argv);
		return 0;

	} else if (strcmp(subcommand, "test") == 0) {
		bool success = cm_run_func_tests(argc, argv);

		if (! success) {
			return 1;
		}

		return 0;

	} else {
		assert(false && "Invalid subcommand");
	}
}
