#ifndef CM_STRINGVIEW_H
#define CM_STRINGVIEW_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


typedef struct CMStringView
{
	const char *data;
	size_t len;
} CMStringView;


CMStringView CM_SV_NULL = (CMStringView) {0};


typedef bool (*CMCharPredicate)(char x);


CMStringView cm_sv (const char *cstr)
{
	return (CMStringView) {
        .data = cstr,
        .len = strlen(cstr)
    };
}


char *cm_sv_to_cstr (CMStringView sv)
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


bool cm_sv_eq (CMStringView sv1, CMStringView sv2)
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
	size_t i = 0;

	while (i < strlen(chars)) {
		if (chars[i] == c) {
			return true;
		}

		i += 1;
	}

	return false;
}


size_t cm_trim_left (CMStringView *sv, const char *chars)
{
	size_t i = 0;

	while (i < sv->len && cm_in_chars(sv->data[i], chars)) {
		i += 1;
	}

	sv->data = sv->data + i;
	sv->len = sv->len - i;

	return i;
}


size_t cm_trim_left_ws (CMStringView *sv)
{
	size_t i = 0;

	while (i < sv->len && isspace(sv->data[i])) {
		i += 1;
	}

	sv->data = sv->data + i;
	sv->len = sv->len - i;

	return i;
}


bool cm_starts_with (const CMStringView sv, const CMStringView prefix)
{
	if (prefix.len > sv.len) {
		return false;
	}

	for (size_t i = 0; i < prefix.len; i++) {
		if (prefix.data[i] != sv.data[i]) {
			return false;
		}
	}

	return true;
}


CMStringView cm_chop_left_while (CMStringView *sv, CMCharPredicate predicate)
{
	size_t chop_len = 0;

	while (predicate(sv->data[chop_len])) {
		chop_len++;
	}

	CMStringView chopped = {
		.data = sv->data,
		.len = chop_len,
	};

	sv->data = sv->data + chop_len;
	sv->len = sv->len - chop_len;

	return chopped;
}


CMStringView cm_chop_left_delim (CMStringView *sv, CMStringView delim)
{
	size_t chop_len = 0;
	bool found = false;

	while (delim.len + chop_len <= sv->len) {
		if (memcmp(sv->data + chop_len, delim.data, delim.len) == 0) {
			found = true;
			break;
		}

		chop_len++;
	}

	if (! found) {
		return CM_SV_NULL;
	}

	CMStringView chopped = {
		.data = sv->data,
		.len = chop_len,
	};

	sv->data = sv->data + chop_len + delim.len;
	sv->len = sv->len - chop_len - delim.len;

	return chopped;
}


CMStringView cm_chop_left_len (CMStringView *sv, size_t chop_len)
{
	assert(chop_len <= sv->len);

	CMStringView chopped = {
		.data = sv->data,
		.len = chop_len,
	};

	sv->data = sv->data + chop_len;
	sv->len = sv->len - chop_len;

	return chopped;
}


bool cm_sv_empty (CMStringView sv)
{
	return sv.len == 0;
}


#endif // CM_STRINGVIEW_H
