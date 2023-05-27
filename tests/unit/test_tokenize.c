
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "unit.h"
#include "../../src/tokenize.h"


bool cm_test_sv ()
{
	const char *cstr = "test";
	struct CMStringView sv = cm_sv(cstr);

	for (int i = 0; i < sv.len; i += 1) {
		if (sv.data[i] != cstr[i]) {
			cm_test_error("Character %c does not match %c in position %d\n", sv.data[i], cstr[i], i);
			return false;
		}
	}

	if (strlen(cstr) != sv.len) {
		cm_test_error("Length of sv %zu does not match length of cstring %zu\n", sv.len, strlen(cstr));
		return false;
	}

	return true;
}



void cm_test_tokenize ()
{
	cm_add_test(cm_test_sv);
}
