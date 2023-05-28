#ifndef CM_TOKENIZE_H
#define CM_TOKENIZE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef CM_SV_CAPACITY
#define CM_SV_CAPACITY (1024 * 1024)
#endif

typedef struct CMStringView
{
	const char *data;
	size_t len;
} CMStringView;


CMStringView cm_sv (const char *cstr)
{
	return (CMStringView) {
        .data = cstr,
        .len = strlen(cstr)
    };
}


const char *cm_sv_to_cstr (CMStringView sv)
{
	char *cstr = (char *)malloc(sv.len + 1);  // +1 for null terminator

	if (cstr == NULL) {
		return NULL;
	}

	sprintf(cstr, "%.*s", (int) sv.len, sv.data);

	return cstr;
}


bool cm_sv_cmp_cstr (CMStringView sv, const char *cstr)
{
	if (sv.len != strlen(cstr)) {
		return false;
	}

	for (size_t i = 0; i < sv.len; i++) {
		if (sv.data[i] != cstr[i]) {
			return false;
		}
	}

	return true;
}


bool cm_sv_cmp (CMStringView sv1, CMStringView sv2)
{
	if (sv1.len != sv2.len) {
		return false;
	}

	for (size_t i = 0; i < sv1.len; i++) {
		if (sv1.data[i] != sv2.data[i]) {
			return false;
		}
	}

	return true;
}


bool cm_in_chars (char c, const char *chars)
{
	int i = 0;

	while (i < strlen(chars)) {
		if (chars[i] == c) {
			return true;
		}

		i += 1;
	}

	return false;
}


CMStringView cm_trim_left (const CMStringView sv, const char *chars)
{
	int i = 0;

	while (i < sv.len && cm_in_chars(sv.data[i], chars)) {
		i += 1;
	}

	return (CMStringView) {
		.data = sv.data + i,
		.len = sv.len - i
	};
}


CMStringView cm_trim_left_ws (const CMStringView sv)
{
	int i = 0;

	while (i < sv.len && isspace(sv.data[i])) {
		i += 1;
	}

	return (CMStringView) {
		.data = sv.data + i,
		.len = sv.len - i
	};
}


#endif // CM_TOKENIZE_H
