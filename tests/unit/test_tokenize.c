
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "unit.h"
#include "../../src/tokenize.h"


typedef struct CMTestInChars {
	char c;
	const char *chars;
	bool result;
} CMTestInChars;


typedef struct CMTestSvCstr {
	CMStringView sv;
	const char *cstr;
	bool result;
} CMTestSvCstr;


typedef struct CMTestSvSv {
	CMStringView sv1;
	CMStringView sv2;
	bool result;
} CMTestSvSv;


bool cm_test_sv ()
{
	const char *cstr = "test";
	CMStringView sv = cm_sv(cstr);

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


bool cm_test_cv_cmp_cstr ()
{
	CMTestSvCstr tests[] = {
		{cm_sv("abc"), "abc", true},
		{cm_sv(""), "", true},
		{cm_sv("abd"), "abc", false},
		{cm_sv("abcd"), "abc", false},
		{cm_sv("abc"), "abcd", false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvCstr); i += 1) {
		if (cm_sv_cmp_cstr(tests[i].sv, tests[i].cstr) != tests[i].result) {
			cm_test_error("Failed in_chars test %s, %s, %d\n", tests[i].sv.data, tests[i].cstr, tests[i].result);
			return false;
		}
	}

	return true;
}


bool cm_test_sv_cmp ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("abc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv("abd"), cm_sv("abc"), false},
		{cm_sv("abcd"), cm_sv("abc"), false},
		{cm_sv("abc"), cm_sv("abcd"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		if (cm_sv_cmp(tests[i].sv1, tests[i].sv2) != tests[i].result) {
			cm_test_error("Failed in_chars test %s, %s, %d\n", tests[i].sv1.data, tests[i].sv2.data, tests[i].result);
			return false;
		}
	}

	return true;
}


bool cm_test_sv_to_cstr ()
{
	CMStringView sv = cm_sv("test");
	const char *cstr = cm_sv_to_cstr(sv);

	if (strcmp(cstr, sv.data) != 0) {
		cm_test_error("Failed conversion to cstring");
		return false;
	}

	return true;
}


bool cm_test_in_chars ()
{
	CMTestInChars tests[] = {
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


bool cm_test_trim_left ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("  \t\nabc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv("  \t\n  abc"), cm_sv("\t abc"), true},
		{cm_sv(" abc"), cm_sv(" abd"), false},
		{cm_sv("abc"), cm_sv("abcd"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		CMStringView sv1 = cm_trim_left(tests[i].sv1, " \t\n");
		CMStringView sv2 = cm_trim_left(tests[i].sv2, " \t\n");

		if (cm_sv_cmp(sv1, sv2) != tests[i].result) {
			cm_test_error("Failed in_chars test %s, %s, %d\n", tests[i].sv1.data, tests[i].sv2.data, tests[i].result);
			return false;
		}
	}

	return true;
}


bool cm_test_trim_left_ws ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("  \t\nabc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv("  \t\n  abc"), cm_sv("\t abc"), true},
		{cm_sv(" abc"), cm_sv(" abd"), false},
		{cm_sv("abc"), cm_sv("abcd"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		CMStringView sv1 = cm_trim_left_ws(tests[i].sv1);
		CMStringView sv2 = cm_trim_left_ws(tests[i].sv2);

		if (cm_sv_cmp(sv1, sv2) != tests[i].result) {
			cm_test_error("Failed in_chars test %s, %s, %d\n", tests[i].sv1.data, tests[i].sv2.data, tests[i].result);
			return false;
		}
	}

	return true;
}


void cm_test_tokenize ()
{
	cm_add_test(cm_test_sv);
	cm_add_test(cm_test_sv_to_cstr);
	cm_add_test(cm_test_sv_cmp);
	cm_add_test(cm_test_cv_cmp_cstr);
	cm_add_test(cm_test_in_chars);
	cm_add_test(cm_test_trim_left);
	cm_add_test(cm_test_trim_left_ws);
}
