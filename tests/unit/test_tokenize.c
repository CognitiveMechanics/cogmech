
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "unit.h"
#include "../../src/tokenize.h"


struct CMTestInChars {
	char c;
	const char *chars;
	bool result;
};


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


bool cm_test_in_chars ()
{
	struct CMTestInChars tests[] = {
		{'c', "abc", true},
		{'\t', "\t\n ", true},
		{'c', "\t\n ", false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestInChars); i += 1) {
		if (cm_in_chars(tests[i].c, tests[i].chars) != tests[i].result) {
			cm_test_error("Failed in_chars test %c, %s, %d\n", tests[i].c, tests[i].chars, tests[i].result);
			return false;
		}
	}

	return true;
}


void cm_test_tokenize ()
{
	cm_add_test(cm_test_sv);
	cm_add_test(cm_test_in_chars);
}
