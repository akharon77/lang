#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "parser.h"
#include "tree.h"

const char *GetGeneral(const char *str, TreeNode *value)
{
    str = GetExpression(str, value);

    assert(*str == '\0');
    ++str;

    return str;
}

const char *GetExpression(const char *str, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();
    str = GetProduct(str, top_node);

    while (*str == '+' || *str == '-')
    {
        char op = *str;
        ++str;

        TreeNode *buf_val = TreeNodeNew();
        str = GetProduct(str, buf_val);

        switch (op)
        {
            case '+':
                top_node = ADD(top_node, buf_val);
                break;
            case '-':
                top_node = SUB(top_node, buf_val);
                break;
            default:
                assert(0 && "Syntax error\n");
        }
    }

    *value = *top_node;
    free(top_node);

    return str;
}

const char *GetProduct(const char *str, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();
    str = GetPower(str, top_node);

    while (*str == '*' || *str == '/')
    {
        char op = *str;
        ++str;

        TreeNode *buf_val = TreeNodeNew();
        str = GetPower(str, buf_val);

        switch (op)
        {
            case '*':
                top_node = MUL(top_node, buf_val);
                break;
            case '/':
                top_node = DIV(top_node, buf_val);
                break;
            default:
                assert(0 && "Syntax error\n");
        }
    }

    *value = *top_node;
    free(top_node);

    return str;
}

const char *GetPower(const char *str, TreeNode *value)
{
    TreeNode *top_node = TreeNodeNew();
    str = GetPrimary(str, top_node);

    while (*str == '^')
    {
        TreeNode *buf_val = TreeNodeNew();

        ++str;
        str = GetPrimary(str, buf_val);

        top_node = EXP(top_node, buf_val);
    }

    *value = *top_node;
    free(top_node);

    return str;
}

void GetPrimary(Stack *stk, TreeNode *value)
{
    if (TestToken(stk, TOK_L_RND_BR))
    {
        NextToken(stk);
        GetExpression(stk, value);
        AssertToken(stk, NULL, R_FIG_BR);
    }
    else if (TestToken(stk, TOK_NUM))
    {
        GetNumber(stk, value);
    }
    else if (TestToken(stk, TOK_NAME))
    {
        Token tok = NextToken(stk);
        if (TestToken(stk, TOK_L_RND_BR))
        {
            StackPush(stk, &tok);
            GetFunction(stk, value);
        }
        else
        {
            
        }
    }
    else
    {
        assert(0 && "GetPrimary parsing error\n");
    }
}

void GetNumber(Stack *stk, TreeNode *value)
{
    Token tok = {};
    TokenCtor(&tok);

    AssertToken(stk, &tok, TOK_NUM);

    NUM_CTOR(value, tok.val.num);

    TokenDtor(&tok);

    return str;
}

void GetVariable(Stack *stk, TreeNode *value)
{
    Token tok = {};
    TokenCtor(&tok);

    AssertToken(stk, TOK_NAME, &tok);

    TreeNodeCtor(value,
                 TREE_NODE_TYPE_VAR,
                 {.var = strdup(tok.val.name)},
                 NULL, NULL);

    TokenDtor(&tok);

    return str;
}

void GetFunction(Stack *stk, TreeNode *value)
{
    int32_t op = 0;

    if (strncmp(str, "sin", 3) == 0)
    {
        op = OP_SIN;
        str += 3;
    }
    else if (strncmp(str, "cos", 3) == 0)
    {
        op = OP_COS;
        str += 3;
    }
    else if (strncmp(str, "ln", 2) == 0)
    {
        op = OP_LN;
        str += 2;
    }
    else
        assert(0 && "Wrong function");

    assert(*str == '(');
    ++str;

    TreeNode *arg = TreeNodeNew();
    str = GetExpression(str, arg);

    assert(*str == ')');
    ++str;

    OP_CTOR(value, op, NULL, arg);

    return str;
}

