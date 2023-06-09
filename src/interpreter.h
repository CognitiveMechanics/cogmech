
#ifndef CM_INTERPRETER_H
#define CM_INTERPRETER_H

#include <stdbool.h>
#include "parser.h"
#include "context.h"


void cm_print_entity (CMNode *node);
CMNode *cm_create_key_value (CMNode *key, CMNode *value);

CMNode *cm_interpret_entity (CMContext *context, CMNode *node);
CMNode *cm_interpret_compose (CMContext *context, CMNode *node);
CMNode *cm_interpret_extract (CMContext *context, CMNode *node);
CMNode *cm_interpret_transclude (CMContext *context, CMNode *node);
CMNode *cm_interpret_match (CMContext *context, CMNode *node);
CMNode *cm_interpret_increment (CMContext *context, CMNode *node);
CMNode *cm_interpret_decrement (CMContext *context, CMNode *node);
CMNode *cm_interpret_dot (CMContext *context, CMNode *node);
CMNode *cm_interpret_eval (CMContext *context, CMNode *node);
CMNode *cm_interpret_op_invoke (CMContext *context, CMNode *node);
CMNode *cm_interpret_entity (CMContext *context, CMNode *node);

void cm_interpret_symbol_def (CMContext *context, CMNode *node);
void cm_interpret_op_def (CMContext *context, CMNode *node);
void cm_interpret_relation_def (CMContext *context, CMNode *node);
void cm_interpret_print (CMContext *context, CMNode *node);
void cm_interpret_include (CMContext *context, CMNode *node);

void cm_interpret_file (CMContext *context, const char *filename);
void cm_interpret (CMContext *context, CMNode *ast);


#endif // CM_INTERPRETER_H
