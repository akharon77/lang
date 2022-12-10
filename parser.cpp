#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "parser.h"
#include "tree.h"
#include "dsl.h"

void GetExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetAssignExpression(stk, top_node);

    if (TestToken(stk, TOK_COMMA))
    {
        NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetAssignExpression(stk, next_val);

        top_node = CreateTreeNode(TREE_NODE_TYPE_DEFS,
                       {.var = NULL},
                       top_node, next_val);
    }

    *value = *top_node;
    free(top_node);
}

void GetAssignExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetLogicalExpression(stk, top_node);
    
    if (TestToken(stk, TOK_ASS))
    {
        NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetAssignExpression(stk, next_val);

        top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                       {.op = OP_TYPE_ASS},
                       top_node, next_val);
    }

    *value = *top_node;
    free(top_node);
}

void GetLogicalExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetLogicalOrExpression(stk, top_node);

    if (TestToken(stk, TOK_INTRG))
    {
        NextToken(stk);

        TreeNode *true_expr = TreeNodeNew();
        GetExpression(stk, true_expr);

        AssertToken(stk, NULL, TOK_TWDTS);
        
        TreeNode *false_expr = TreeNodeNew();
        GetLogicalExpression(stk, false_expr);

        top_node = CreateTreeNode(TREE_NODE_TYPE_CALL,
                       {.var = NULL},
                       CreateTreeNode(TREE_NODE_TYPE_FUNC, {.var = strdup("ternary")}, NULL, NULL),
                    ARG(value, ARG(true_expr, ARG(false_expr, NULL))));
    }

    *value = *top_node;
    free(top_node);
}

void GetLogicalOrExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetLogicalAndExpression(stk, top_node);

    while (TestToken(stk, TOK_OR))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetLogicalAndExpression(stk, next_val);

        top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                       {.op = OP_TYPE_OR},
                       top_node, next_val);
    }

    *value = *top_node;
    free(top_node);
}

void GetLogicalAndExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetEqualExpression(stk, top_node);

    while (TestToken(stk, TOK_AND))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetEqualExpression(stk, next_val);

        top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                       {.op = OP_TYPE_AND},
                       top_node, next_val);
    }

    *value = *top_node;
    free(top_node);
}

void GetEqualExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetRelativeExpression(stk, top_node);

    while (TestToken(stk, TOK_EQ) || TestToken(stk, TOK_NEQ))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetRelativeExpression(stk, next_val);

        switch (tok.id)
        {
            case TOK_EQ:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_EQ},
                               top_node, next_val);
                break;
            case TOK_NEQ:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_NEQ},
                               top_node, next_val);
                break;
        }
    }

    *value = *top_node;
    free(top_node);
}

void GetRelativeExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetAdditiveExpression(stk, top_node);
    
    while (TestToken(stk, TOK_LES) || TestToken(stk, TOK_LEQ) ||
           TestToken(stk, TOK_GER) || TestToken(stk, TOK_GEQ))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetAdditiveExpression(stk, next_val);
            
        switch (tok.id)
        {
            case TOK_LES:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_LES},
                               top_node, next_val);
                break;
            case TOK_LEQ:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_LEQ},
                               top_node, next_val);
                break;
            case TOK_GER:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_GER},
                               top_node, next_val);
                break;
            case TOK_GEQ:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_GEQ},
                               top_node, next_val);
                break;
        }
    }

    *value = *top_node;
    free(top_node);
}

void GetAdditiveExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetMultiplicativeExpression(stk, top_node);

    while (TestToken(stk, TOK_PLUS) || TestToken(stk, TOK_MINUS))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetMultiplicativeExpression(stk, next_val);

        switch (tok.id)
        {
            case TOK_PLUS:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_ADD},
                               top_node, next_val);
                break;
            case TOK_MINUS:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_SUB},
                               top_node, next_val);
                break;
        }
    }

    *value = *top_node;
    free(top_node);
}

void GetMultiplicativeExpression(Stack *stk, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();

    GetUnaryExpression(stk, top_node);

    while (TestToken(stk, TOK_MUL) || TestToken(stk, TOK_DIV) || TestToken(stk, TOK_PRCNT))
    {
        Token tok = NextToken(stk);

        TreeNode *next_val = TreeNodeNew();
        GetUnaryExpression(stk, next_val);

        switch (tok.id)
        {
            case TOK_MUL:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_MUL},
                               top_node, next_val);
                break;
            case TOK_DIV:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_DIV},
                               top_node, next_val);
                break;
            case TOK_PRCNT:
                top_node = CreateTreeNode(TREE_NODE_TYPE_OP,
                               {.op = OP_TYPE_MOD},
                               top_node, next_val);
                break;
        }
    }

    *value = *top_node;
    free(top_node);
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
        GetUnaryExpression(stk, value);
    }
    else if (TestToken(stk, TOK_NOT))
    {
        NextToken(stk);
        
        TreeNode *res = TreeNodeNew();
        GetUnaryExpression(stk, res);

        OP_CTOR(value, OP_TYPE_NOT, NULL, res);
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

        char *name = strdup(value->value.var);

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
    TreeNode *top_node = TreeNodeNew();

    GetAssignExpression(stk, top_node);

    if (TestToken(stk, TOK_COMMA))
    {
        NextToken(stk);

        TreeNode *next_arg = TreeNodeNew();
        GetListExpressions(stk, next_arg);

        top_node = CreateTreeNode(TREE_NODE_TYPE_ARG,
                                  {.var = NULL},
                                  top_node, next_arg);
    }
    else
    {
        top_node = CreateTreeNode(TREE_NODE_TYPE_ARG,
                                  {.var = NULL},
                                  top_node, NULL);
    }

    *value = *top_node;
    free(top_node);
}

void GetPrimaryExpression(Stack *stk, TreeNode *value)
{
    if (TestToken(stk, TOK_IDENT))
    {
        GetIdent(stk, value);
    }
    else if (TestToken(stk, TOK_CONST))
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

void AssertToken(Stack *stk, Token *tok, int32_t tok_id)
{
    Token buf_token = {};
    if (tok == NULL)
        tok = &buf_token;

    StackPop(stk, tok);
    assert(tok->id == tok_id);
}

