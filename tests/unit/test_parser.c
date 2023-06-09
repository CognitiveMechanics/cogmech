

#include "unit.h"
#include "test_parser.h"
#include "../../src/parser.h"


bool test_cm_parse_compose (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_LT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_LT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_GT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_GT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_expr(&list);

	if (composition->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("invalid composition expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of composition should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (composition->children[2]->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("third element of composition should be CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (composition->children[2]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("inner element of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_extract (void)
{
	CMTokenList list = cm_tokenlist();

	CMNode *symbol = cm_node_symbol(cm_sv("symbol"));

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_extract(symbol, &list);

	if (composition->type != CM_NODE_TYPE_EXTRACT) {
		cm_test_error("invalid extract expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of composition should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_transclude (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);
	keyword.value = cm_sv("T");

	CMToken tokens[] = {
		keyword,
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_transclude(&list);

	if (composition->type != CM_NODE_TYPE_TRANSCLUDE) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of transclusion should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (composition->children[2]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("third element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_op_def (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken name = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);
	name.value = cm_sv("O");

	CMToken tokens[] = {
		name,
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_S_ARROW
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *op = cm_parse_op_def(&list);

	if (op->type != CM_NODE_TYPE_OP_DEF) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (! cm_sv_eq(op->value, cm_sv("O"))) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (op->children[0]->type != CM_NODE_TYPE_OP_ARGLIST) {
		cm_test_error("first element of op should be CM_NODE_TYPE_OP_ARGLIST\n");
		return false;
	}

	if (op->children[0]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("arguments should be of type CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (op->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("body of op should be CM_TOKEN_TYPE_WORD\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_op_invoke (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken name = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);
	name.value = cm_sv("O");

	CMToken tokens[] = {
		name,
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *op = cm_parse_op_invoke(&list);

	if (op->type != CM_NODE_TYPE_OP_INVOKE) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (! cm_sv_eq(op->value, cm_sv("O"))) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (op->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of op should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (op->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of op should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (op->children[2]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("third element of op should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_expr_list(void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_expr_list(
		&list,
		CM_NODE_TYPE_TRANSCLUDE,
		CM_TOKEN_TYPE_PAREN_IN,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	if (composition->type != CM_NODE_TYPE_TRANSCLUDE) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second element of transclusion should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (composition->children[2]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("third element of transclusion should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_match (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PERCENT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_COMMA
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_PAREN_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *composition = cm_parse_match(&list);

	if (composition->type != CM_NODE_TYPE_MATCH) {
		cm_test_error("invalid transclude expression\n");
		return false;
	}

	if (composition->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first element of match should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (composition->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("second element of match should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_class (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_IN
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_DOT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_SQ_BRACKET_OUT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *parsed = cm_parse_class(&list);

	if (parsed->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("classes should be of type CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (parsed->n_children != 2) {
		cm_test_error("classes should have two children\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("first child should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_DOT_PROXY) {
		cm_test_error("second child should be CM_NODE_TYPE_DOT_PROXY\n");
		return false;
	}

	return true;
}


bool test_cm_parse_key (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_HASH
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *parsed = cm_parse_key(&list);

	if (parsed->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("classes should be of type CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (parsed->n_children != 2) {
		cm_test_error("classes should have two children\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_KEY) {
		cm_test_error("first child should be CM_NODE_TYPE_KEY\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("second child should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	return true;
}


bool test_cm_parse_dot (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_DOT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *parsed = cm_parse_dot(&list);

	if (parsed->type != CM_NODE_TYPE_DOT) {
		cm_test_error("should be of type CM_NODE_TYPE_DOT\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("dot should have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("child should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	return true;
}


bool test_cm_parse_int (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_INT
	);

	token.value = cm_sv("1");

	cm_tokenlist_append(&list, token);

	CMNode *node = cm_parse_int(&list);

	if (node->type != CM_NODE_TYPE_INT) {
		cm_test_error("node should be CM_NODE_TYPE_INT\n");
		return false;
	}

	if (cm_node_int_value(node) != 1) {
		cm_test_error("node value should be 1\n");
		return false;
	}

	return true;
}


bool test_cm_parse_word (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken token = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	token.value = cm_sv("test_word");
	cm_tokenlist_append(&list, token);

	CMNode *word = cm_parse_word(&list);

	if (word->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid word type\n");
		return false;
	}

	if (! cm_sv_eq(word->value, cm_sv("test_word"))) {
		cm_test_error("invalid word value\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_expr (void)
{
	CMTokenList list = cm_tokenlist();

	CMToken tokens[] = {
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_QUOTED
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_LT
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_WORD
		),
		cm_token(
			"filename.cogm",
			0,
			0,
			CM_TOKEN_TYPE_GT
		),
	};

	for (size_t i = 0; i < ARRAY_LEN(tokens); i++) {
		cm_tokenlist_append(&list, tokens[i]);
	}

	CMNode *symbol = cm_parse_expr(&list);
	CMNode *literal = cm_parse_expr(&list);
	CMNode *composition = cm_parse_expr(&list);

	if (symbol->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol expression\n");
		return false;
	}

	if (literal->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal expression\n");
		return false;
	}

	if (composition->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("invalid composition expression\n");
		return false;
	}

	if (! cm_tokenlist_empty(list)) {
		cm_test_error("list not empty\n");
		return false;
	}

	return true;
}


bool test_cm_parse_symbol_def (void)
{
	CMToken symbol = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	symbol.value = cm_sv("a");

	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON_EQ
	);

	CMToken literal = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_QUOTED
	);

	literal.value = cm_sv("string");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, symbol);
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, literal);

	CMNode *parsed = cm_parse_symbol_def(&list);

	if (parsed->type != CM_NODE_TYPE_SYMBOL_DEF) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("a"))) {
		cm_test_error("invalid symbol node value\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->value, cm_sv("string"))) {
		cm_test_error("invalid literal node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse_relation_def (void)
{
	CMToken symbol = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	symbol.value = cm_sv("a");

	CMToken endl = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_ENDL
	);

	CMToken colon = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON
	);

	CMToken lt = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_LT
	);

	CMToken state = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	state.value = cm_sv("s0");

	CMToken gt = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_GT
	);

	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_D_ARROW
	);

	CMToken literal = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_QUOTED
	);

	literal.value = cm_sv("string");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, symbol);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, colon);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, lt);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, state);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, gt);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, endl);
	cm_tokenlist_append(&list, literal);

	CMNode *parsed = cm_parse_relation_def(&list);

	if (parsed->type != CM_NODE_TYPE_RELATION_DEF) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("a"))) {
		cm_test_error("invalid symbol node value\n");
		return false;
	}

	if (parsed->children[1]->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("invalid state node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[0]->value, cm_sv("s0"))) {
		cm_test_error("invalid state node value\n");
		return false;
	}

	if (parsed->children[2]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("invalid literal node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[2]->value, cm_sv("string"))) {
		cm_test_error("invalid literal node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse_eval (void)
{
	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	keyword.value = cm_sv("R");

	CMToken paren_in = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_IN
	);

	CMToken input = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	CMToken paren_out = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, keyword);
	cm_tokenlist_append(&list, paren_in);
	cm_tokenlist_append(&list, input);
	cm_tokenlist_append(&list, paren_out);

	CMNode *parsed = cm_parse_eval(&list);

	if (parsed->type != CM_NODE_TYPE_EVAL) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("node should only have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	return true;
}


bool test_cm_parse_increment (void)
{
	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PLUS
	);

	CMToken paren_in = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_IN
	);

	CMToken input = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	CMToken paren_out = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, keyword);
	cm_tokenlist_append(&list, paren_in);
	cm_tokenlist_append(&list, input);
	cm_tokenlist_append(&list, paren_out);

	CMNode *parsed = cm_parse_increment(&list);

	if (parsed->type != CM_NODE_TYPE_INCREMENT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("node should only have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	return true;
}


bool test_cm_parse_decrement (void)
{
	CMToken keyword = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_MINUS
	);

	CMToken paren_in = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_IN
	);

	CMToken input = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	CMToken paren_out = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_PAREN_OUT
	);

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, keyword);
	cm_tokenlist_append(&list, paren_in);
	cm_tokenlist_append(&list, input);
	cm_tokenlist_append(&list, paren_out);

	CMNode *parsed = cm_parse_decrement(&list);

	if (parsed->type != CM_NODE_TYPE_DECREMENT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->n_children != 1) {
		cm_test_error("node should only have one child\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	return true;
}


bool test_cm_parse_print (void)
{
	CMToken op = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_COLON
	);

	CMToken symbol = cm_token(
		"filename.cogm",
		0,
		0,
		CM_TOKEN_TYPE_WORD
	);

	symbol.value = cm_sv("a");

	CMTokenList list = cm_tokenlist();
	cm_tokenlist_append(&list, op);
	cm_tokenlist_append(&list, symbol);

	CMNode *parsed = cm_parse_print(&list);

	if (parsed->type != CM_NODE_TYPE_PRINT) {
		cm_test_error("invalid node type\n");
		return false;
	}

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("invalid symbol node type\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->value, cm_sv("a"))) {
		cm_test_error("invalid symbol node value\n");
		return false;
	}

	return true;
}


bool test_cm_parse (void)
{
	CMContext context = cm_context();
	CMTokenList list = cm_tokenize_file(&context, "../cogm/examples/00-hello.cogm");
	CMNode *parsed = cm_parse(&list);

	// root

	if (parsed->type != CM_NODE_TYPE_ROOT) {
		cm_test_error("invalid root node type\n");
		return false;
	}

	// a := "hello"

	if (parsed->children[0]->type != CM_NODE_TYPE_SYMBOL_DEF) {
		cm_test_error("invalid assignment of first symbol\n");
		return false;
	}

	if (parsed->children[0]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("left hand of first assignment should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->children[0]->value, cm_sv("a"))) {
		cm_test_error("first symbol should be 'a'\n");
		return false;
	}

	if (parsed->children[0]->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("right hand of first assignment should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[0]->children[1]->value, cm_sv("hello"))) {
		cm_test_error("value of first assignment should be 'hello'\n");
		return false;
	}

	// b := <a, "world">

	if (parsed->children[1]->type != CM_NODE_TYPE_SYMBOL_DEF) {
		cm_test_error("invalid assignment of second symbol\n");
		return false;
	}

	if (parsed->children[1]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("left hand of second assignment should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[0]->value, cm_sv("b"))) {
		cm_test_error("second symbol should be 'b'\n");
		return false;
	}

	if (parsed->children[1]->children[1]->type != CM_NODE_TYPE_COMPOSE) {
		cm_test_error("right hand of second assignment should be CM_NODE_TYPE_COMPOSE\n");
		return false;
	}

	if (parsed->children[1]->children[1]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("first component of composition should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[1]->children[0]->value, cm_sv("a"))) {
		cm_test_error("first component of composition should be 'a'\n");
		return false;
	}

	if (parsed->children[1]->children[1]->children[1]->type != CM_NODE_TYPE_LITERAL) {
		cm_test_error("second component of composition should be CM_NODE_TYPE_LITERAL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[1]->children[1]->children[1]->value, cm_sv("world"))) {
		cm_test_error("second component of composition should be 'world'\n");
		return false;
	}

	// : b

	if (parsed->children[2]->type != CM_NODE_TYPE_PRINT) {
		cm_test_error("invalid print statment\n");
		return false;
	}

	if (parsed->children[2]->children[0]->type != CM_NODE_TYPE_SYMBOL) {
		cm_test_error("operand of print statement should be CM_NODE_TYPE_SYMBOL\n");
		return false;
	}

	if (! cm_sv_eq(parsed->children[2]->children[0]->value, cm_sv("b"))) {
		cm_test_error("print should be called with 'b'\n");
		return false;
	}

	cm_tokenlist_free(&list);
	cm_node_free(parsed);

	return true;
}


void test_cm_parser (void)
{
	printf("Loading parser tests...\n");

	cm_add_test(test_cm_parse_word);
	cm_add_test(test_cm_parse_int);
	cm_add_test(test_cm_parse_expr);
	cm_add_test(test_cm_parse_compose);
	cm_add_test(test_cm_parse_extract);
	cm_add_test(test_cm_parse_transclude);
	cm_add_test(test_cm_parse_increment);
	cm_add_test(test_cm_parse_decrement);
	cm_add_test(test_cm_parse_op_invoke);
	cm_add_test(test_cm_parse_op_def);
	cm_add_test(test_cm_parse_expr_list);
	cm_add_test(test_cm_parse_match);
	cm_add_test(test_cm_parse_class);
	cm_add_test(test_cm_parse_dot);
	cm_add_test(test_cm_parse_key);
	cm_add_test(test_cm_parse_symbol_def);
	cm_add_test(test_cm_parse_relation_def);
	cm_add_test(test_cm_parse_eval);
	cm_add_test(test_cm_parse_print);
	cm_add_test(test_cm_parse);
}
