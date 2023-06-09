
#ifndef CM_PARSER_H
#define CM_PARSER_H

#include <stdlib.h>
#include <stdio.h>

#include "tokenizer.h"
#include "node.h"


CMNode *cm_parse_compose (CMTokenList *list);
CMNode *cm_parse_word (CMTokenList *list);
CMNode *cm_parse_int (CMTokenList *list);
CMNode *cm_parse_expr (CMTokenList *list);
CMNode *cm_parse_increment (CMTokenList *list);
CMNode *cm_parse_decrement (CMTokenList *list);
CMNode *cm_parse_expr_list (CMTokenList *list, CMNodeType node_type, CMTokenType start_token_type, CMTokenType end_token_type);
CMNode *cm_parse_extract (CMNode *subject, CMTokenList *list);
CMNode *cm_parse_transclude (CMTokenList *list);
CMNode *cm_parse_match (CMTokenList *list);
CMNode *cm_parse_eval (CMTokenList *list);
CMNode *cm_parse_op_invoke (CMTokenList *list);
CMNode *cm_parse_builtin (CMTokenList *list);
CMNode *cm_parse_class (CMTokenList *list);
CMNode *cm_parse_key (CMTokenList *list);
CMNode *cm_parse_dot (CMTokenList *list);
CMNode *cm_parse_expr (CMTokenList *list);
CMNode *cm_parse_compose (CMTokenList *list);
CMNode *cm_parse_symbol_def (CMTokenList *list);
CMNode *cm_parse_op_def (CMTokenList *list);
CMNode *cm_parse_relation_def (CMTokenList *list);
CMNode *cm_parse_print (CMTokenList *list);
CMNode *cm_parse (CMTokenList *list);


#endif

