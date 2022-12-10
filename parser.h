#ifndef PARSER_H
#define PARSER_H

#include "tree.h"
#include "tokenizer.h"
#include "stack.h"

void GetExpression               (Stack *stk, TreeNode *value);
void GetAssignExpression         (Stack *stk, TreeNode *value);
void GetLogicalExpression        (Stack *stk, TreeNode *value);
void GetLogicalOrExpression      (Stack *stk, TreeNode *value);
void GetLogicalAndExpression     (Stack *stk, TreeNode *value);
void GetEqualExpression          (Stack *stk, TreeNode *value);
void GetRelativeExpression       (Stack *stk, TreeNode *value);
void GetAdditiveExpression       (Stack *stk, TreeNode *value);
void GetMultiplicativeExpression (Stack *stk, TreeNode *value);
void GetUnaryExpression          (Stack *stk, TreeNode *value);
void GetFunctionExpression       (Stack *stk, TreeNode *value);
void GetListExpressions          (Stack *stk, TreeNode *value);
void GetPrimaryExpression        (Stack *stk, TreeNode *value);
void GetIdent                    (Stack *stk, TreeNode *value);
void GetNumber                   (Stack *stk, TreeNode *value);

void AssertToken                 (Stack *stk, Token *tok, int32_t tok_id);

#endif  // PARSER_H

