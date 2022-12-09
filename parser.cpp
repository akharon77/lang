#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "parser.h"
#include "tree.h"
#include "dsl.h"

void GetAssignExpression(Stack *stk, TreeNode *value)
{
    GetLogicalExpression(stk, value);
    
    if (TestToken(stk, TOK_ASS))
    {
        NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetAssignExpression(stk, next_val);

        TreeNodeCtor(value, TREE_NODE_TYPE_ASS
    }
}

void GetLogicalExpression(Stack *stk, TreeNode *value)
{
    GetLogicalOrExpression(stk, value);

    if (TestToken(stk, TOK_INTRG))
    {
        NextToken(stk);

        TreeNode *true_expr = NodeNew();
        GetExpression(stk, true_expr);

        AssertToken(stk, NULL, TOK_TWDTS);
        
        TreeNode *false_expr = NodeNew();
        GetLogicalExpression(stk, false_expr);

        TreeNodeCtor(value, TREE_NODE_TYPE_CALL,
                    {.var = NULL},
                    CreateTreeNode(TREE_NODE_TYPE_FUNC, {.var = strdup("ternary")}, NULL, NULL),
                    ARG(value, ARG(true_expr, ARG(false_expr, NULL))));
    }
}

void GetLogicalOrExpression(Stack *stk, TreeNode *value)
{
    GetLogicalAndExpression(stk, value);

    while (TestToken(stk, TOK_OR))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetLogicalAndExpression(stk, next_val);

        TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                     {.op = OP_TYPE_OR},
                     value, next_val);
    }
}

void GetLogicalAndExpression(Stack *stk, TreeNode *value)
{
    GetEqualExpression(stk, value);

    while (TestToken(stk, TOK_AND))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetEqualExpression(stk, next_val);

        TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                     {.op = OP_TYPE_AND},
                     value, next_val);
    }
}

void GetEqualExpression(Stack *stk, TreeNode *value)
{
    GetRelativeExpression(stk, value);

    while (TestToken(stk, TOK_EQ) || TestToken(stk, TOK_NEQ))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetRelativeExpression(stk, next_val);

        switch (tok.id)
        {
            case TOK_EQ:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_EQ},
                             value, next_val);
                break;
            case TOK_NEQ:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_NEQ},
                             value, next_val);
                break;
        }
    }
}

void GetRelativeExpression(Stack *stk, TreeNode *value)
{
    GetAdditiveExpression(stk, value);
    
    while (TestToken(stk, TOK_LES) || TestToken(stk, TOK_LEQ) ||
           TestToken(stk, TOK_GER) || TestToken(stk, TOK_GEQ))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetAdditiveExpression(stk, next_val);
            
        switch (tok.id)
        {
            case TOK_LES:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_LES},
                             value, next_val);
                break;
            case TOK_LEQ:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_LEQ},
                             value, next_val);
                break;
            case TOK_GER:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_GER},
                             value, next_val);
                break;
            case TOK_GEQ:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_GEQ},
                             value, next_val);
                break;
        }
    }
}

void GetAdditiveExpression(Stack *stk, TreeNode *value)
{
    GetMultiplicativeExpression(stk, value);

    while (TestToken(stk, TOK_PLUS) || TestToken(stk, TOK_MINUS))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetMultiplicativeExpression(stk, next_val);

        switch (tok.id)
        {
            case TOK_PLUS:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_ADD},
                             value, next_val);
                break;
            case TOK_MINUS:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_SUB},
                             value, next_val);
                break;
        }
    }
}

void GetMultiplicativeExpression(Stack *stk, TreeNode *value)
{
    GetUnaryExpression(stk, value);

    while (TestToken(stk, TOK_MUL) || TestToken(stk, TOK_DIV) || TestToken(stk, TOK_PRCNT))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetUnaryExpression(stk, next_val);

        switch (tok.id)
        {
            case TOK_MUL:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_MUL},
                             value, next_val);
                break;
            case TOK_DIV:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_DIV},
                             value, next_val);
                break;
            case TOK_PRCNT:
                TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                             {.op = OP_TYPE_PRCNT},
                             value, next_val);
                break;
        }
    }
}

void GetUnaryExpression(Stack *stk, TreeNode *value)
{
    if (TestToken(stk, TOK_MINUS))
    {
        NextToken(stk);

        TreeNode *res = TreeNodeNew();
        GetUnaryExpression(stk, res);

        OP_CTOR(value, OP_TYPE_MUL, CREATE_NUM(-1), res);
    }
    else if (TestToken(stk, TOK_PLUS))
    {
        NextToken(stk);
        GetUnaryExpression(skt, value);
    }
    else if (TestToken(stk, TOK_DEN))
    {
        NextToken(stk);
        
        TreeNode *res = TreeNodeNew();
        GetUnaryExpression(stk, res);

        OP_CTOR(value, OP_TYPE_DEN, NULL, res);
    }
    else
    {
        GetFunctionExpression(stk, value);
    }
}

void GetFunctionExpression(Stack *stk, TreeNode *value)
{
    GetPrimaryExpression(stk, value);

    if (TestToken(stk, TOK_L_RND_BR))
    {
        NextToken(stk);

        char *name = strdup(value.value.var);

        TreeNode *args = TreeNodeNew();
        GetListExpressions(stk, args);
        TreeNodeCtor(value, TREE_NODE_TYPE_CALL, 
                     {.var = NULL},
                     CreateTreeNode(TREE_NODE_TYPE_FUNC, {.var = name}, NULL, NULL),
                     args);

        AssertToken(stk, NULL, TOK_R_RND_BR);
    }
}

void GetListExpressions(Stack *stk, TreeNode *value)
{
    GetAssignExpression(stk, value);

    if (TestToken(stk, TOK_COMMA))
    {
        NextToken(stk);

        TreeNode *next_arg = NodeNew();
        GetListExpressions(stk, next_arg);

        TreeNodeCtor(value, TREE_NODE_TYPE_ARG, 
                    {.var = NULL},
                    value, next_arg);
    }
}

void GetPrimaryExpression(Stack *stk, TreeNode *value)
{
    if (TestToken(stk, TOK_TYPE_IDENT))
    {
        GetIdent(stk, value);
    }
    else if (TestToken(stk, TOK_TYPE_CONST))
    {
        GetNumber(stk, value);
    }
    else if (TestToken(stk, TOK_L_RND_BR))
    {
        NextToken(stk);
        GetExpression(stk, value);
        AssertToken(stk, NULL, TOK_R_RND_BR);
    }
}

void GetIdent(Stack *stk, TreeNode *value)
{
    Token tok = NextToken(stk);
    TreeNodeCtor(value, TREE_NODE_TYPE_VAR,
                 {.var = strdup(tok.val.name)},
                 NULL, NULL);

    TokenDtor(&tok);
}

void GetNumber(Stack *stk, TreeNode *value)
{
    Token tok = NextToken(stk);
    TreeNodeCtor(value, TREE_NODE_TYPE_NUM,
                {.dbl = tok.val.num},
                NULL, NULL);

    TokenDtor(&tok);
}

