#ifndef CM_TOKENIZE_H
#define CM_TOKENIZE_H

#include <stdbool.h>
#include <string.h>
#include <ctype.h>


struct CMStringView
{
	const char *data;
	size_t len;
};


struct CMStringView cm_sv (const char *cstr)
{
	return (struct CMStringView) {
        .data = cstr,
        .len = strlen(cstr)
    };
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


struct CMStringView cm_trim_left (const struct CMStringView sv, const char *chars)
{
	int i = 0;

	while (i < sv.len && cm_in_chars(sv.data[i], chars)) {
		i += 1;
	}

	return (struct CMStringView) {
		.data = sv.data + i,
		.len = sv.len - i
	};
}


struct CMStringView cm_trim_left_ws (const struct CMStringView sv)
{
	int i = 0;

	while (i < sv.len && isspace(sv.data[i])) {
		i += 1;
	}

	return (struct CMStringView) {
		.data = sv.data + i,
		.len = sv.len - i
	};
}


#endif // CM_TOKENIZE_H
