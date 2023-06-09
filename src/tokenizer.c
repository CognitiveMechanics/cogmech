
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

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
	return (bool) isnumber(c) || c == '_';
}


CMTokenList _cm_tokenize (const char *filename, CMStringView sv, size_t *row, size_t *col, CMToken *macro)
{
	assert(CM_TOKEN_TYPE_COUNT == 27);

	CMTokenList list = cm_tokenlist();
	CMContext static_context = cm_context();

	while (! cm_sv_empty(sv)) {
		size_t trimmed = cm_trim_left(&sv, " \t");
		bool is_punctuation = false;
		(*col) += trimmed;

		for (size_t type = 0; type < CM_TOKEN_TYPE_COUNT; type++) {
			const char* symbol = cm_token_type_symbol(type);

			if (symbol != NULL) {
				if (cm_starts_with(sv, cm_sv(symbol))) {
					size_t symbol_len = strlen(symbol);

					cm_tokenlist_append(
						&list,
						cm_token(filename, *row, *col, type)
					);

					cm_chop_left_len(&sv, symbol_len);
					(*col) += symbol_len;
					is_punctuation = true;
					break;
				}
			}
		}

		if (is_punctuation) {
			continue;
		}

		if (cm_starts_with(sv, cm_sv("\""))) {
			CMToken quoted = cm_token(filename, *row, *col, CM_TOKEN_TYPE_QUOTED);
			size_t curr = 1;
			bool terminated = false;

			while (curr < sv.len) {
				if (sv.data[curr] == '"') {
					quoted.value = cm_chop_left_len(&sv, curr + 1);
					terminated = true;
					break;
				}

				curr += 1;
			}

			(*col) += quoted.value.len;

			if (!terminated) {
				quoted.value = cm_chop_left_len(&sv, curr);
				cm_syntax_error(quoted, "Unterminated quote");
			}

			// remove quotes
			quoted.value.data = quoted.value.data + 1;
			quoted.value.len = quoted.value.len - 2;

			cm_tokenlist_append(&list, quoted);

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
			CMToken word = cm_token(filename, *row, *col, CM_TOKEN_TYPE_WORD);
			word.value = cm_chop_left_while(&sv, cm_is_word);
			(*col) += word.value.len;

			(*col) += cm_trim_left(&sv, " \t");

			if (cm_starts_with(sv, cm_sv("::"))) {
				if (cm_static_context_has_macro(&static_context, word.value)) {
					cm_syntax_error(word, "Macro redefined");
				}

				(*col) += 2;
				cm_chop_left_delim(&sv, cm_sv("::"));

				cm_static_context_def_macro(&static_context, word, _cm_tokenize(filename, sv, row, col, &word));

				cm_chop_left_delim(&sv, cm_sv("\n"));

			} else if (cm_static_context_has_macro(&static_context, word.value)) {
				CMMacroDef def = cm_static_context_get_macro(&static_context, word.value);

				for (size_t i = 0; i < cm_tokenlist_len(def.body); i++) {
					CMToken expanded = cm_tokenlist_get(def.body, i);
					expanded.macro = &word;
					cm_tokenlist_append(&list, expanded);
				}

			} else {
				cm_tokenlist_append(&list, word);
			}

		} else if (isnumber(sv.data[0])) {
			CMToken word = cm_token(filename, *row, *col, CM_TOKEN_TYPE_INT);
			word.value = cm_chop_left_while(&sv, cm_is_num);

			cm_tokenlist_append(&list, word);
			(*col) += word.value.len;

		} else if (cm_in_chars(sv.data[0], "\n")) {
			CMToken word = cm_token(filename, *row, *col, CM_TOKEN_TYPE_ENDL);
			cm_chop_left_len(&sv, 1);

			(*row) += 1;
			(*col) = 0;

			if (macro) {
				break;
			}

			cm_tokenlist_append(&list, word);

		} else if (cm_in_chars(sv.data[0], "\r")) {
			// just skip carriage return
			cm_trim_left(&sv, "\r");

		} else {
			CMToken unknown = cm_token(filename, *row, *col, CM_TOKEN_TYPE_UNKNOWN);
			unknown.value = cm_chop_left_while(&sv, cm_is_not_space);
			cm_syntax_error(unknown, "Invalid token");
		}
	}

	return list;
}


CMTokenList cm_tokenize (const char *filename, CMStringView sv)
{
	size_t row = 0;
	size_t col = 0;

	return _cm_tokenize(filename, sv, &row, &col, false);
}


CMTokenList cm_tokenize_file (const char *filename)
{
	return cm_tokenize(
		filename,
		cm_sv(cm_read_file_to_cstr(filename))
	);
}
