#ifndef PARSER_H
#define PARSER_H

#include "tree.h"
#include "tokenizer.h"
#include "stack.h"

#define AssertToken(STK, TOK, TOK_ID)                                   \
do                                                                      \
{                                                                       \
    Token *tok = TOK;                                                   \
    Token buf_token = {};                                               \
    if (tok == NULL)                                                    \
        tok = &buf_token;                                               \
                                                                        \
    StackPop(STK, tok);                                                 \
    if (tok->id != TOK_ID)                                              \
        fprintf(stderr,                                                 \
                "Token assertion failed. Expected " #TOK_ID "\n"        \
                "Obtained %d\n"                                         \
                "File: %s, line: %d\n"                                  \
                "Function: %s\n",                                       \
                tok->id,                                                \
                __FILE__, __LINE__,                                     \
                __PRETTY_FUNCTION__);                                   \
}                                                                       \
while (0)

void GetStatementsList           (Stack *stk, TreeNode *value);
void GetDefinitionStatement      (Stack *stk, TreeNode *value);
void GetStatement                (Stack *stk, TreeNode *value);
void GetNewVarStatement          (Stack *stk, TreeNode *value);
void GetReturnStatement          (Stack *stk, TreeNode *value);
void GetFunctionStatement        (Stack *stk, TreeNode *value);
void GetWhileStatement           (Stack *stk, TreeNode *value);
void GetBlockStatement           (Stack *stk, TreeNode *value);
void GetIfStatement              (Stack *stk, TreeNode *value);
void GetExpressionStatement      (Stack *stk, TreeNode *value);
void GetAssignStatement          (Stack *stk, TreeNode *value);
void GetExpression               (Stack *stk, TreeNode *value);
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
void GetParamList                (Stack *stk, TreeNode *value);
void GetPrimaryExpression        (Stack *stk, TreeNode *value);
void GetIdent                    (Stack *stk, TreeNode *value);
void GetNumber                   (Stack *stk, TreeNode *value);

// void AssertToken                 (Stack *stk, Token *tok, int32_t tok_id);

#endif  // PARSER_H

