
#ifndef CM_TEST_STRINGVIEW_H
#define CM_TEST_STRINGVIEW_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "unit.h"
#include "../../src/stringview.h"


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


bool test_cm_sv ()
{
	const char *cstr = "test";
	CMStringView sv = cm_sv(cstr);

	for (int i = 0; i < sv.len; i += 1) {
		if (sv.data[i] != cstr[i]) {
			cm_test_error("test_cm_sv character %c does not match %c in position %d\n", sv.data[i], cstr[i], i);
			return false;
		}
	}

	if (strlen(cstr) != sv.len) {
		cm_test_error("test_cm_sv length of sv %zu does not match length of cstring %zu\n", sv.len, strlen(cstr));
		return false;
	}

	return true;
}


bool test_cm_cv_cmp_cstr ()
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
			cm_test_error("test_cm_cv_cmp_cstr test: %.*s, %s, %d\n", (int) tests[i].sv.len, tests[i].sv.data, tests[i].cstr, tests[i].result);
			return false;
		}
	}

	return true;
}


bool test_cm_sv_eq ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("abc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv("abd"), cm_sv("abc"), false},
		{cm_sv("abcd"), cm_sv("abc"), false},
		{cm_sv("abc"), cm_sv("abcd"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		if (cm_sv_eq(tests[i].sv1, tests[i].sv2) != tests[i].result) {
			cm_test_error("test_cm_sv_eq test: %.*s, %.*s, %d\n", (int) tests[i].sv1.len, tests[i].sv1.data, (int) tests[i].sv2.len, tests[i].sv2.data, tests[i].result);
			return false;
		}
	}

	return true;
}


bool test_cm_sv_to_cstr ()
{
	CMStringView sv = cm_sv("test");
	char *cstr = cm_sv_to_cstr(sv);

	if (strcmp(cstr, sv.data) != 0) {
		free(cstr);
		cm_test_error("test_cm_sv_to_cstr conversion to cstring");
		return false;
	}

	free(cstr);
	return true;
}


bool test_cm_in_chars ()
{
	CMTestInChars tests[] = {
		{'c', "abc", true},
		{'\t', "\t\n ", true},
		{'c', "\t\n ", false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestInChars); i += 1) {
		if (cm_in_chars(tests[i].c, tests[i].chars) != tests[i].result) {
			cm_test_error("test_cm_in_chars test: %c, %s, %d\n", tests[i].c, tests[i].chars, tests[i].result);
			return false;
		}
	}

	return true;
}


bool test_cm_trim_left ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("  \t\nabc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv("  \t\n  abc"), cm_sv("\t abc"), true},
		{cm_sv(" abc"), cm_sv(" abd"), false},
		{cm_sv("abc"), cm_sv("abcd"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		CMStringView sv1 = tests[i].sv1;
		CMStringView sv2 = tests[i].sv2;

		size_t len1 = sv1.len;
		size_t len2 = sv2.len;

		size_t trimmed1 = cm_trim_left(&sv1, " \t\n");
		size_t trimmed2 = cm_trim_left(&sv2, " \t\n");

		if (cm_sv_eq(sv1, sv2) != tests[i].result) {
			cm_test_error("test_cm_trim_left test: %.*s, %.*s, %d\n", (int) tests[i].sv1.len, tests[i].sv1.data, (int) tests[i].sv2.len, tests[i].sv2.data, tests[i].result);
			return false;
		}

		if (sv1.len != len1 - trimmed1) {
			cm_test_error("test_cm_trim_left: invalid trimmed length %zu in string one", trimmed1);
			return false;
		}

		if (sv2.len != len2 - trimmed2) {
			cm_test_error("test_cm_trim_left: invalid trimmed length %zu in string two", trimmed2);
			return false;
		}
	}

	return true;
}


bool test_cm_trim_left_ws ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("  \t\nabc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv("  \t\n  abc"), cm_sv("\t abc"), true},
		{cm_sv(" abc"), cm_sv(" abd"), false},
		{cm_sv("abc"), cm_sv("abcd"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		CMStringView sv1 = tests[i].sv1;
		CMStringView sv2 = tests[i].sv2;

		size_t len1 = sv1.len;
		size_t len2 = sv2.len;

		size_t trimmed1 = cm_trim_left_ws(&sv1);
		size_t trimmed2 = cm_trim_left_ws(&sv2);

		if (cm_sv_eq(sv1, sv2) != tests[i].result) {
			cm_test_error("test_cm_trim_left test: %s, %s, %d\n", tests[i].sv1.data, tests[i].sv2.data, tests[i].result);
			return false;
		}

		if (sv1.len != len1 - trimmed1) {
			cm_test_error("test_cm_trim_left: invalid trimmed length %zu in string one", trimmed1);
			return false;
		}

		if (sv2.len != len2 - trimmed2) {
			cm_test_error("test_cm_trim_left: invalid trimmed length %zu in string two", trimmed2);
			return false;
		}
	}

	return true;
}


bool test_cm_starts_with ()
{
	CMTestSvSv tests[] = {
		{cm_sv("abc"), cm_sv("abc"), true},
		{cm_sv(""), cm_sv(""), true},
		{cm_sv(" abc"), cm_sv(" abd"), false},
		{cm_sv("abcd"), cm_sv("abc"), true},
		{cm_sv("ab"), cm_sv("abc"), false},
	};

	for (size_t i = 0; i < sizeof(tests) / sizeof(struct CMTestSvSv); i += 1) {
		CMStringView sv1 = tests[i].sv1;
		CMStringView sv2 = tests[i].sv2;

		if (cm_starts_with(sv1, sv2) != tests[i].result) {
			cm_test_error("test_cm_starts_with test: %.*s, %.*s, %d\n", (int) tests[i].sv1.len, tests[i].sv1.data, (int) tests[i].sv2.len, tests[i].sv2.data, tests[i].result);
			return false;
		}
	}

	return true;
}


bool test_cm_char_isspace (char c)
{
	return (bool) isspace(c);
}


bool test_cm_chop_left_while ()
{
	CMStringView sv1 = cm_sv("   abc");
	CMStringView sv2 = cm_sv("abc");
	CMStringView chopped = cm_chop_left_while(&sv1, test_cm_char_isspace);

	if (! cm_sv_eq(sv1, sv2)) {
		cm_test_error("test_cm_sv_chop_left_while test: %.*s, %.*s\n", (int) sv1.len, sv1.data, (int) sv2.len, sv2.data);
		return false;
	}

	if (! cm_sv_eq(chopped, cm_sv("   "))) {
		cm_test_error("test_cm_sv_chop_left_while chopped: %.*s\n", (int) chopped.len, chopped.data);
		return false;
	}

	return true;
}


bool test_cm_chop_left_delim ()
{
	CMStringView sv = cm_sv("abc--def");
	CMStringView delim = cm_sv("--");
	CMStringView chopped = cm_chop_left_delim(&sv, delim);

	if (! cm_sv_eq(chopped, cm_sv("abc"))) {
		cm_test_error("test_cm_chop_left_delim chopped: %.*s\n", (int) chopped.len, chopped.data);
		return false;
	}

	if (! cm_sv_eq(sv, cm_sv("def"))) {
		cm_test_error("test_cm_chop_left_delim sv: %.*sn", (int) sv.len, sv.data);
		return false;
	}

	return true;
}


bool test_cm_chop_left_len ()
{
	CMStringView sv = cm_sv("abc--def");
	CMStringView chopped = cm_chop_left_len(&sv, 5);

	if (! cm_sv_eq(chopped, cm_sv("abc--"))) {
		cm_test_error("test_cm_chop_left_len chopped: %.*s\n", (int) chopped.len, chopped.data);
		return false;
	}

	if (! cm_sv_eq(sv, cm_sv("def"))) {
		cm_test_error("test_cm_chop_left_len sv: %.*s\n", (int) sv.len, sv.data);
		return false;
	}

	return true;
}


bool test_cm_sv_empty ()
{
	if (! cm_sv_empty(cm_sv(""))) {
		cm_test_error("test_cm_sv_empty failed on empty string\n");

		return false;
	}

	if (cm_sv_empty(cm_sv("a"))) {
		cm_test_error("test_cm_sv_empty failed on nonempty string\n");

		return false;
	}

	return true;
}


void test_cm_stringview ()
{
	printf("Loading stringview tests...\n");

	cm_add_test(test_cm_sv);
	cm_add_test(test_cm_sv_to_cstr);
	cm_add_test(test_cm_sv_eq);
	cm_add_test(test_cm_cv_cmp_cstr);
	cm_add_test(test_cm_in_chars);
	cm_add_test(test_cm_trim_left);
	cm_add_test(test_cm_trim_left_ws);
	cm_add_test(test_cm_starts_with);
	cm_add_test(test_cm_chop_left_while);
	cm_add_test(test_cm_chop_left_delim);
	cm_add_test(test_cm_chop_left_len);
	cm_add_test(test_cm_sv_empty);
}

#endif // CM_TEST_STRINGVIEW_H
