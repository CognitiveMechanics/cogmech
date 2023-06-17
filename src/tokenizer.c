
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <libgen.h>

#include "file.h"
#include "stringview.h"
#include "tokenizer.h"


void  cm_syntax_error (CMToken token, const char *message)
{
	fflush(stdout);

	if (! cm_sv_eq(token.value, CM_SV_NULL)) {
		fprintf(stderr, "ERROR %s:%zu:%zu: %s(%.*s): %s",
			token.loc.filename, token.loc.row + 1, token.loc.col + 1,
			 cm_readable_token_type(token.type),
			(int) token.value.len, token.value.data,
			message
		);
	} else {
		fprintf(stderr, "ERROR %s:%zu:%zu: %s: %s",
			token.loc.filename, token.loc.row + 1, token.loc.col + 1,
			cm_readable_token_type(token.type),
			message
		);
	}

	if (token.macro) {
		fprintf(stderr, "\nExpanded from %s:%zu:%zu: %s(%.*s)",
			token.macro->loc.filename, token.macro->loc.row + 1, token.macro->loc.col + 1,
			cm_readable_token_type(token.macro->type),
			(int) token.macro->value.len, token.macro->value.data
		);
	}

	exit(CM_ERROR_EXIT_SYNTAX);
}


bool cm_is_word (char c)
{
	return (bool) isalnum(c) || c == '_';
}


bool cm_is_not_space (char c)
{
	return (bool) ! isspace(c);
}


bool cm_is_num (char c)
{
	return (bool) isdigit(c) || c == '_';
}


// TODO: test
CMToken cm_tokenize_quote (CMLoc *loc, CMStringView *sv)
{
	CMToken quoted = cm_token(loc->filename, loc->row, loc->col, CM_TOKEN_TYPE_QUOTED);
	size_t curr = 1;
	bool terminated = false;

	while (curr < sv->len) {
		if (sv->data[curr] == '"') {
			quoted.value = cm_chop_left_len(sv, curr + 1);
			terminated = true;
			break;
		}

		curr += 1;
	}

	loc->col += quoted.value.len;

	if (!terminated) {
		quoted.value = cm_chop_left_len(sv, curr);
		cm_syntax_error(quoted, "Unterminated quote");
	}

	// remove quotes
	quoted.value.data = quoted.value.data + 1;
	quoted.value.len = quoted.value.len - 2;

	return quoted;
}


// TODO: test
CMToken cm_tokenize_int (CMLoc *loc, CMStringView *sv)
{
	CMToken word = cm_token(loc->filename, loc->row, loc->col, CM_TOKEN_TYPE_INT);
	word.value = cm_chop_left_while(sv, cm_is_num);
	loc->col += word.value.len;

	return word;
}


CMTokenList _cm_tokenize (CMContext *context, CMLoc *loc, CMStringView sv, CMToken *macro)
{
	assert(CM_TOKEN_TYPE_COUNT == 26);

	CMTokenList list = cm_tokenlist();

	while (! cm_sv_empty(sv)) {
		size_t trimmed = cm_trim_left(&sv, " \t");
		bool is_punctuation = false;
		loc->col += trimmed;

		for (size_t type = 0; type < CM_TOKEN_TYPE_COUNT; type++) {
			const char* symbol = cm_token_type_symbol(type);

			if (symbol != NULL) {
				if (cm_starts_with(sv, cm_sv(symbol))) {
					size_t symbol_len = strlen(symbol);

					cm_tokenlist_append(
						&list,
						cm_token(loc->filename, loc->row, loc->col, type)
					);

					cm_chop_left_len(&sv, symbol_len);
					loc->col += symbol_len;
					is_punctuation = true;
					break;
				}
			}
		}

		if (is_punctuation) {
			continue;
		}

		if (cm_starts_with(sv, cm_sv("\""))) {
			CMToken quoted = cm_tokenize_quote(loc, &sv);
			cm_tokenlist_append(&list, quoted);

		} else if (cm_starts_with(sv, cm_sv("\""))) {
			CMToken quoted = cm_tokenize_quote(loc, &sv);
			cm_tokenlist_append(&list, quoted);

		} else if (cm_starts_with(sv, cm_sv("@include"))) {
			cm_chop_left_delim(&sv, cm_sv("@include"));
			loc->col += strlen("@include");
			loc->col += cm_trim_left(&sv, " \t");
			CMToken quoted = cm_tokenize_quote(loc, &sv);

			char filename_buffer[1024] = {0};

			if (cm_starts_with(quoted.value, cm_sv("/"))) {
				strcat(filename_buffer, cm_sv_to_cstr(quoted.value));
			} else {
				strcat(filename_buffer, dirname((char *) loc->filename));
				strcat(filename_buffer, "/");
				strcat(filename_buffer, cm_sv_to_cstr(quoted.value));
			}

			CMTokenList included = cm_tokenize_file(context, (const char *) filename_buffer);

			for (size_t i = 0; i < cm_tokenlist_len(included); i++) {
				cm_tokenlist_append(&list, cm_tokenlist_get(included, i));
			}

		} else if (cm_starts_with(sv, cm_sv("@trace"))) {
			cm_chop_left_delim(&sv, cm_sv("@trace"));
			loc->col += strlen("@trace");
			loc->col += cm_trim_left(&sv, " \t");

			CMToken token = cm_tokenize_int(loc, &sv);

			if (token.value.len == 0) {
				cm_syntax_error(token, "Invalid trace level");
			}

			context->trace = strtol(cm_sv_to_cstr(token.value), NULL, 10);

		} else if (cm_starts_with(sv, cm_sv("//"))) {
			size_t curr = 2;

			while (curr < sv.len) {
				if (sv.data[curr] == '\n') {
					break;
				}

				curr += 1;
			}

			cm_chop_left_len(&sv, curr);

		} else if (isalpha(sv.data[0])) {
			CMToken word = cm_token(loc->filename, loc->row, loc->col, CM_TOKEN_TYPE_WORD);
			word.value = cm_chop_left_while(&sv, cm_is_word);
			loc->col += word.value.len;
			loc->col += cm_trim_left(&sv, " \t");

			if (cm_starts_with(sv, cm_sv("::"))) {
				if (cm_static_context_has_macro(context, word.value)) {
					cm_syntax_error(word, "Macro redefined");
				}

				loc->col += 2;
				cm_chop_left_delim(&sv, cm_sv("::"));
				cm_static_context_def_macro(context, word, _cm_tokenize(context, loc, sv,  &word));
				cm_chop_left_delim(&sv, cm_sv("\n"));

			} else if (cm_static_context_has_macro(context, word.value)) {
				CMMacroDef def = cm_static_context_get_macro(context, word.value);

				for (size_t i = 0; i < cm_tokenlist_len(def.body); i++) {
					CMToken expanded = cm_tokenlist_get(def.body, i);
					expanded.macro = malloc(sizeof(CMToken));
					(*expanded.macro) = word;
					cm_tokenlist_append(&list, expanded);
				}

			} else {
				cm_tokenlist_append(&list, word);
			}

		} else if (isdigit(sv.data[0])) {
			CMToken word = cm_tokenize_int(loc, &sv);
			cm_tokenlist_append(&list, word);

		} else if (cm_in_chars(sv.data[0], "\n")) {
			CMToken word = cm_token(loc->filename, loc->row, loc->col, CM_TOKEN_TYPE_ENDL);
			cm_chop_left_len(&sv, 1);

			loc->row += 1;
			loc->col = 0;

			if (macro) {
				break;
			}

			cm_tokenlist_append(&list, word);

		} else if (cm_in_chars(sv.data[0], "\r")) {
			// just skip carriage return
			cm_trim_left(&sv, "\r");

		} else {
			CMToken unknown = cm_token(loc->filename, loc->row, loc->col, CM_TOKEN_TYPE_UNKNOWN);
			unknown.value = cm_chop_left_while(&sv, cm_is_not_space);
			cm_syntax_error(unknown, "Invalid token");
		}
	}

	return list;
}


CMTokenList cm_tokenize (CMContext *context, const char *filename, CMStringView sv)
{
	CMLoc *loc = malloc(sizeof(CMLoc));
	*loc = cm_loc(filename, 0, 0);

	return _cm_tokenize(context, loc, sv, false);
}


CMTokenList cm_tokenize_file (CMContext *context, const char *filename)
{
	return cm_tokenize(
		context,
		filename,
		cm_sv(cm_read_file_to_cstr(filename))
	);
}
