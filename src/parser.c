
#include "stringview.h"
#include "tokenizer.h"
#include "parser.h"


static const CMTokenType CM_FMT_ASSIGNMENT[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_COLON_EQ,
};

static const CMTokenType CM_FMT_RELATION[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_D_ARROW,
};

static const CMTokenType CM_FMT_RELATION_ALIASED[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_COLON,
};

static const CMTokenType CM_FMT_OP_START[] = {
	CM_TOKEN_TYPE_WORD,
	CM_TOKEN_TYPE_PAREN_IN,
};


CMNode *cm_parse_extract (CMNode *subject, CMTokenList *list)
{
	CMNode *extraction = cm_node(CM_NODE_TYPE_EXTRACT);
	cm_node_set_token(extraction, subject->token);
	cm_node_append_child(extraction, subject);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_IN); // shift [
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_tokenlist_skip_endl(list);

	cm_node_append_child(extraction, expr);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_OUT); // shift ]

	return extraction;
}


CMNode *cm_parse_expr_list (CMTokenList *list, CMNodeType node_type, CMTokenType start_token_type, CMTokenType end_token_type)
{
	CMToken start = cm_tokenlist_expect(list, start_token_type);
	CMNode *node = cm_node(node_type);
	cm_node_set_token(node, start);

	do {
		cm_tokenlist_skip_endl(list);

		cm_node_append_child(
			node,
			cm_parse_expr(list)
		);

		cm_tokenlist_skip_endl(list);

		CMToken next_token = cm_tokenlist_first(*list);

		if (next_token.type == CM_TOKEN_TYPE_COMMA) {
			cm_tokenlist_shift(list);
			continue;
		} else {
			break;
		}
	} while (! cm_tokenlist_empty(*list));

	cm_tokenlist_skip_endl(list);
	cm_tokenlist_expect(list, end_token_type);

	return node;
}


CMNode *cm_parse_transclude (CMTokenList *list)
{
	CMToken op = cm_tokenlist_shift(list);

	assert(op.type == CM_TOKEN_TYPE_WORD);
	assert(cm_sv_eq(op.value, cm_sv(cm_node_type_word(CM_NODE_TYPE_TRANSCLUDE))));
	assert(cm_tokenlist_len(*list) >= 4);

	CMNode *new_node = cm_parse_expr_list(list, CM_NODE_TYPE_TRANSCLUDE, CM_TOKEN_TYPE_PAREN_IN, CM_TOKEN_TYPE_PAREN_OUT);
	cm_node_set_token(new_node, op);

	if (new_node->n_children != 3) {
		cm_syntax_error(op, "Transclude accepts exactly 3 arguments");
	}

	CMNode *entity = new_node->children[0];

	if (entity->type != CM_NODE_TYPE_COMPOSE && entity->type != CM_NODE_TYPE_SYMBOL) {
		cm_syntax_error(entity->token, "Transcluded nodes must be composed entities or symbols");
	}

	return new_node;
}


CMNode *cm_parse_match (CMTokenList *list)
{
	CMToken first_token = cm_tokenlist_shift(list);

	CMNode *match = cm_parse_expr_list(list, CM_NODE_TYPE_MATCH, CM_TOKEN_TYPE_PAREN_IN, CM_TOKEN_TYPE_PAREN_OUT);
	cm_node_set_token(match, first_token);

	if (match->n_children != 2) {
		cm_syntax_error(first_token, "match accepts exactly two arguments");
	}

	return match;
}


CMNode *cm_parse_eval (CMTokenList *list)
{
	CMToken word = cm_tokenlist_expect(list, CM_TOKEN_TYPE_WORD);

	if (! cm_sv_eq(word.value, cm_sv(cm_node_type_word(CM_NODE_TYPE_EVAL)))) {
		assert(false && "Invalid eval word");
	}

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_IN);
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_tokenlist_skip_endl(list);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_OUT);

	CMNode *eval = cm_node(CM_NODE_TYPE_EVAL);
	cm_node_set_token(eval, word);
	cm_node_append_child(eval, expr);

	return eval;
}


CMNode *cm_parse_increment (CMTokenList *list)
{
	CMToken symbol = cm_tokenlist_expect(list, CM_TOKEN_TYPE_PLUS);
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_IN);
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_tokenlist_skip_endl(list);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_OUT);

	CMNode *op = cm_node(CM_NODE_TYPE_INCREMENT);
	cm_node_set_token(op, symbol);
	cm_node_append_child(op, expr);

	return op;
}


CMNode *cm_parse_decrement (CMTokenList *list)
{
	CMToken symbol = cm_tokenlist_expect(list, CM_TOKEN_TYPE_MINUS);
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_IN);
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_tokenlist_skip_endl(list);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_PAREN_OUT);

	CMNode *op = cm_node(CM_NODE_TYPE_DECREMENT);
	cm_node_set_token(op, symbol);
	cm_node_append_child(op, expr);

	return op;
}


CMNode *cm_parse_op_invoke (CMTokenList *list)
{
	if (! cm_tokenlist_like(*list, CM_FMT_OP_START)) {
		assert(false && "Invalid token list for op definition");
	}

	CMToken symbol_token = cm_tokenlist_shift(list);

	CMNode *def = cm_parse_expr_list(
		list,
		CM_NODE_TYPE_OP_INVOKE,
		CM_TOKEN_TYPE_PAREN_IN,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	cm_node_set_token(def, symbol_token);

	def->value = symbol_token.value;

	return def;
}


CMNode *cm_parse_builtin (CMTokenList *list)
{
	CMToken token = cm_tokenlist_first(*list);
	CMNode *builtin = cm_node_from_word(token.value);
	cm_node_set_token(builtin, token);

	assert(builtin);

	switch (builtin->type) {
		case CM_NODE_TYPE_TRANSCLUDE: {
			return cm_parse_transclude(list);
		}

		case CM_NODE_TYPE_EVAL: {
			return cm_parse_eval(list);
		}

		default: {
			assert(false && "Invalid builtin word");
		}
	}
}


CMNode *cm_parse_class (CMTokenList *list)
{
	CMToken start = cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_IN);
	bool is_dot = false;

	if (cm_tokenlist_first_like(*list, CM_TOKEN_TYPE_DOT)) {
		cm_tokenlist_shift(list);
		is_dot = true;
	}

	CMToken word = cm_tokenlist_expect(list, CM_TOKEN_TYPE_WORD);
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_SQ_BRACKET_OUT);

	CMNode *composition = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_set_token(composition, start);

	CMNode *literal = cm_node_literal(word.value);
	cm_node_set_token(literal, word);
	cm_node_append_child(composition, literal);

	if (is_dot) {
		CMNode *proxy = cm_node(CM_NODE_TYPE_DOT_PROXY);
		cm_node_set_token(proxy, start);
		cm_node_append_child(composition, proxy);
	} else {
		CMNode *proxy = cm_node(CM_NODE_TYPE_PROXY);
		cm_node_set_token(proxy, start);
		cm_node_append_child(composition, proxy);
	}

	return composition;
}


CMNode *cm_parse_key (CMTokenList *list)
{
	CMToken start = cm_tokenlist_expect(list, CM_TOKEN_TYPE_HASH);
	CMNode *expr = cm_parse_expr(list);

	CMNode *composition = cm_node(CM_NODE_TYPE_COMPOSE);
	cm_node_set_token(composition, start);

	CMNode *key = cm_node(CM_NODE_TYPE_KEY);
	cm_node_set_token(key, start);

	cm_node_append_child(composition, key);
	cm_node_append_child(composition, expr);

	return composition;
}


CMNode *cm_parse_dot (CMTokenList *list)
{
	CMToken dot_token = cm_tokenlist_expect(list, CM_TOKEN_TYPE_DOT);
	CMToken next = cm_tokenlist_first(*list);

	switch (next.type) {
		case CM_TOKEN_TYPE_WORD: {
			CMNode *dot = cm_node(CM_NODE_TYPE_DOT);
			cm_node_set_token(dot, dot_token);
			cm_node_append_child(dot, cm_parse_word(list));

			return dot;
		}

		case CM_TOKEN_TYPE_PROXY: {
			CMNode *dot_proxy = cm_node(CM_NODE_TYPE_DOT_PROXY);
			cm_tokenlist_shift(list);
			cm_node_set_token(dot_proxy, dot_token);

			return dot_proxy;
		}


		case CM_TOKEN_TYPE_INT: {
			CMNode *exact_int = cm_parse_int(list);
			exact_int->type = CM_NODE_TYPE_INT_EXACT;

			return exact_int;
		}

		default: {
			cm_syntax_error(
				next,
				"Expected CM_TOKEN_TYPE_WORD, CM_TOKEN_TYPE_INT, "
				"or CM_TOKEN_TYPE_PROXY after CM_TOKEN_TYPE_DOT"
			);
		}
	}

	assert(false && "Unreachable");
}


CMNode *cm_parse_word (CMTokenList *list)
{
	CMToken token = cm_tokenlist_first(*list);
	assert(token.type == CM_TOKEN_TYPE_WORD);

	CMNode *builtin = cm_node_from_word(token.value);

	if (builtin) {
		return cm_parse_builtin(list);
	}

	if (cm_tokenlist_len(*list) > 1) {
		CMToken next_token = cm_tokenlist_get(*list, 1);

		if (next_token.type == CM_TOKEN_TYPE_SQ_BRACKET_IN) {
			cm_tokenlist_shift(list);
			return cm_parse_extract(cm_node_symbol(token.value), list);
		} else if (next_token.type == CM_TOKEN_TYPE_PAREN_IN) {
			return cm_parse_op_invoke(list);
		}
	}

	cm_tokenlist_shift(list); // shift word, already in `token`

	CMNode *symbol = cm_node_symbol(token.value);
	cm_node_set_token(symbol, token);

	return symbol;
}


CMNode *cm_parse_int (CMTokenList *list)
{
	CMToken token = cm_tokenlist_shift(list);
	assert(token.type == CM_TOKEN_TYPE_INT);

	CMNode *node = cm_node_int(token.value);
	cm_node_set_token(node, token);

	return node;
}


CMNode *cm_parse_literal (CMTokenList *list)
{
	CMToken token = cm_tokenlist_expect(list, CM_TOKEN_TYPE_QUOTED);
	CMNode *expr = cm_node_literal(token.value);
	cm_node_set_token(expr, token);

	return expr;
}


CMNode *cm_parse_expr (CMTokenList *list)
{
	cm_tokenlist_skip_endl(list);
	CMToken token = cm_tokenlist_first(*list);
	CMNode *expr;

	switch (token.type) {
		case CM_TOKEN_TYPE_DOT: {
			expr = cm_parse_dot(list);
			break;
		}

		case CM_TOKEN_TYPE_HASH: {
			expr = cm_parse_key(list);
			break;
		}

		case CM_TOKEN_TYPE_LT: {
			expr = cm_parse_compose(list);
			break;
		}

		case CM_TOKEN_TYPE_SQ_BRACKET_IN: {
			expr = cm_parse_class(list);
			break;
		}

		case CM_TOKEN_TYPE_PERCENT: {
			expr = cm_parse_match(list);
			break;
		}

		case CM_TOKEN_TYPE_PLUS: {
			expr = cm_parse_increment(list);
			break;
		}

		case CM_TOKEN_TYPE_MINUS: {
			expr = cm_parse_decrement(list);
			break;
		}

		case CM_TOKEN_TYPE_QUOTED: {
			expr = cm_parse_literal(list);
			break;
		}

		case CM_TOKEN_TYPE_PROXY: {
			cm_tokenlist_shift(list);
			expr = cm_node(CM_NODE_TYPE_PROXY);
			cm_node_set_token(expr, token);
			break;
		}

		case CM_TOKEN_TYPE_DOT_PROXY: {
			cm_tokenlist_shift(list);
			expr = cm_node(CM_NODE_TYPE_DOT_PROXY);
			cm_node_set_token(expr, token);
			break;
		}

		case CM_TOKEN_TYPE_TRUE: {
			cm_tokenlist_shift(list);
			expr = cm_node(CM_NODE_TYPE_TRUE);
			cm_node_set_token(expr, token);
			break;
		}

		case CM_TOKEN_TYPE_NULL: {
			cm_tokenlist_shift(list);
			expr = cm_node_null();
			cm_node_set_token(expr, token);
			break;
		}

		case CM_TOKEN_TYPE_WORD: {
			expr = cm_parse_word(list);
			break;
		}

		case CM_TOKEN_TYPE_INT: {
			expr = cm_parse_int(list);
			break;
		}

		default: {
			cm_syntax_error(token, "Expected expression");
		}
	}

	if (! cm_tokenlist_empty(*list) && cm_tokenlist_first(*list).type == CM_TOKEN_TYPE_SQ_BRACKET_IN) {
		return cm_parse_extract(expr, list);
	}

	return expr;
}


CMNode *cm_parse_compose (CMTokenList *list)
{
	return cm_parse_expr_list(list, CM_NODE_TYPE_COMPOSE, CM_TOKEN_TYPE_LT, CM_TOKEN_TYPE_GT);
}


CMNode *cm_parse_symbol_def (CMTokenList *list)
{
	if (! cm_tokenlist_like(*list, CM_FMT_ASSIGNMENT)) {
		assert(false && "Invalid token list for symbol definition");
	}

	CMToken symbol_token = cm_tokenlist_shift(list);
	CMNode *node = cm_node(CM_NODE_TYPE_SYMBOL_DEF);
	cm_node_set_token(node, symbol_token);

	CMNode *symbol = cm_node_symbol(symbol_token.value);
	cm_node_set_token(symbol, symbol_token);
	cm_node_append_child(node, symbol);

	// discard :=
	cm_tokenlist_shift(list);

	CMNode *expr = cm_parse_expr(list);
	cm_node_append_child(node, expr);

	return node;
}


CMNode *cm_parse_op_def (CMTokenList *list)
{
	if (! cm_tokenlist_like(*list, CM_FMT_OP_START)) {
		assert(false && "Invalid token list for op definition");
	}

	CMToken symbol_token = cm_tokenlist_shift(list);

	CMNode *def = cm_node(CM_NODE_TYPE_OP_DEF);
	cm_node_set_token(def, symbol_token);
	def->value = symbol_token.value;

	CMNode *arglist = cm_parse_expr_list(
		list,
		CM_NODE_TYPE_OP_ARGLIST,
		CM_TOKEN_TYPE_PAREN_IN,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	for (size_t i = 0; i < arglist->n_children; i++) {
		if (arglist->children[i]->type != CM_NODE_TYPE_SYMBOL) {
			cm_syntax_error(arglist->children[i]->token, "All arguments in op definition must be symbols");
		}
	}

	cm_node_append_child(def, arglist);

	cm_tokenlist_expect(list, CM_TOKEN_TYPE_S_ARROW);
	cm_node_append_child(def, cm_parse_expr(list));

	return def;
}


CMNode *cm_parse_relation_def (CMTokenList *list)
{
	CMToken start = cm_tokenlist_first(*list);

	bool is_relation = cm_tokenlist_like(*list, CM_FMT_RELATION);
	bool is_aliased_relation = cm_tokenlist_like(*list, CM_FMT_RELATION_ALIASED);

	assert(is_relation || is_aliased_relation);

	CMNode *relation = cm_node(CM_NODE_TYPE_RELATION_DEF);
	cm_node_set_token(relation, start);

	cm_tokenlist_skip_endl(list);

	CMNode *state = cm_parse_word(list);
	cm_node_append_child(relation, state);
	cm_tokenlist_skip_endl(list);

	if (is_aliased_relation) {
		cm_tokenlist_expect(list, CM_TOKEN_TYPE_COLON); // discard :
		CMNode *expr = cm_parse_expr(list);
		cm_node_append_child(relation, expr);
	} else {
		cm_node_append_child(relation, cm_node_null());
	}

	cm_tokenlist_skip_endl(list);
	cm_tokenlist_expect(list, CM_TOKEN_TYPE_D_ARROW); // discard =>
	cm_tokenlist_skip_endl(list);

	CMNode *expr = cm_parse_expr(list);
	cm_node_append_child(relation, expr);

	return relation;
}


CMNode *cm_parse_print (CMTokenList *list)
{
	CMToken start = cm_tokenlist_shift(list);
	CMNode *node = cm_node(CM_NODE_TYPE_PRINT);

	cm_node_set_token(node, start);

	CMNode *expr = cm_parse_expr(list);
	cm_node_append_child(node, expr);

	return node;
}


CMNode *cm_parse (CMTokenList *list)
{
	assert(CM_NODE_TYPE_COUNT == 24);
	assert(CM_TOKEN_TYPE_COUNT == 26);

	CMNode *root = cm_node(CM_NODE_TYPE_ROOT);

	while (! cm_tokenlist_empty(*list)) {
		cm_tokenlist_skip_endl(list);

		if (cm_tokenlist_empty(*list)) {
			break;
		}

		CMToken token = cm_tokenlist_first(*list);

		switch (token.type) {
			case CM_TOKEN_TYPE_WORD: {
				bool is_assignment = cm_tokenlist_like(*list, CM_FMT_ASSIGNMENT);

				if (is_assignment) {
					cm_node_append_child(
						root,
						cm_parse_symbol_def(list)
					);

					break;
				}

				bool is_relation = cm_tokenlist_like(*list, CM_FMT_RELATION);
				bool is_aliased_relation = cm_tokenlist_like(*list, CM_FMT_RELATION_ALIASED);

				if (is_relation || is_aliased_relation) {
					cm_node_append_child(
						root,
						cm_parse_relation_def(list)
					);

					break;
				}

				bool is_op = cm_tokenlist_like(*list, CM_FMT_OP_START);

				if (is_op) {
					cm_node_append_child(
						root,
						cm_parse_op_def(list)
					);

					break;
				}

				cm_syntax_error(token, "A statement that begins with a word must be a definition");
				break;
			}

			case CM_TOKEN_TYPE_COLON: {
				cm_node_append_child(
					root,
					cm_parse_print(list)
				);

				break;
			}

			default: {
				cm_syntax_error(token, "Invalid begin of statement");
			}
		}

		if (! cm_tokenlist_empty(*list)) {
			cm_tokenlist_expect(list, CM_TOKEN_TYPE_ENDL);
			cm_tokenlist_skip_endl(list);
		}
	}

	return root;
}
