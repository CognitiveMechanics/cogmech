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


static CMStringView CM_SV_NULL = (CMStringView) {0};


typedef bool (*CMCharPredicate)(char x);


CMStringView cm_sv (const char *cstr);
char *cm_sv_to_cstr (CMStringView sv);

bool cm_sv_cmp_cstr (CMStringView sv, const char *cstr);
bool cm_sv_eq (CMStringView sv1, CMStringView sv2);

bool cm_in_chars (char c, const char *chars);
size_t cm_trim_left (CMStringView *sv, const char *chars);
size_t cm_trim_left_ws (CMStringView *sv);
bool cm_starts_with (const CMStringView sv, const CMStringView prefix);
CMStringView cm_chop_left_while (CMStringView *sv, CMCharPredicate predicate);
CMStringView cm_chop_left_delim (CMStringView *sv, CMStringView delim);
CMStringView cm_chop_left_len (CMStringView *sv, size_t chop_len);
bool cm_sv_empty (CMStringView sv);


#endif // CM_STRINGVIEW_H
