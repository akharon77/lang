#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "parser.h"
#include "tree.h"
#include "dsl.h"

void GetXorExpression(Stack *stk, TreeNode *value)
{
    GetAndExpression(stk, value);

    while (TestToken(stk, TOK_XOR))
    {
        NextToken(stk);

        TreeNode *next_val = NodeNew();
        GetAndExpression(stk, next_val);

        TreeNodeCtor(value, TREE_NODE_TYPE_OP,
                     {.op = OP_TYPE_XOR},
                     value, next_val);
    }
}


void GetAndExpression(Stack *stk, TreeNode *value)
{
    GetEqualExpression(stk, value);

    while (TestToken(stk, TOK_AND))
    {
        NextToken(stk);

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

void GetFunctionExpression(Stack *stk, TreeNode *value)
{
    Token func_tok = NextToken(stk);
    char *name = strdup(func_tok.val.name);
    TokenDtor(&func_tok);

    AssertToken(stk, NULL, TOK_L_RND_BR);

    TreeNode *args = TreeNodeNew();
    GetListExpressions(stk, args);
    TreeNodeCtor(value, TREE_NODE_TYPE_CALL, NULL, args);

    AssertToken(stk, NULL, TOK_R_RND_BR);
}

void GetListExpressions(Stack *stk, TreeNode *value)
{
    GetExpression(stk, value);

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
        GetIdent(stk, value);
    else if (TestToken(stk, TOK_TYPE_CONST))
        GetConst(stk, value);
    else if (TestToken(stk, TOK_L_RND_BR))
    {
        NextToken(stk);
        GetExpression(stk, value);
        AssertToken(stk, NULL, TOK_R_RND_BR);
    }
}

void GetUnaryExpression(Stack *stk, TreeNode *value)
{
    if (TestToken(stk, TOK_MINUS))
    {
        NextToken(stk);

        TreeNode *res = TreeNodeNew();
        GetExpression(stk, res);

        value = MUL(CREATE_NUM(-1), res);
    }
    else if (TestToken(stk, TOK_PLUS))
    {
        NextToken(stk);
        GetExpression(skt, value);
    }
    else if (TestToken(stk, TOK_DEN))
    {
        NextToken(stk);
        
        TreeNode *res = TreeNodeNew();
        GetExpression(stk, res);

        OP_CTOR(value, OP_TYPE_DEN, NULL, res);
    }
}

// const char *GetGeneral(const char *str, TreeNode *value)
// {
//     str = GetExpression(str, value);
// 
//     assert(*str == '\0');
//     ++str;
// 
//     return str;
// }
// 
// const char *GetExpression(const char *str, TreeNode *value)
// {
//     TreeNode *top_node = TreeNodeNew();
//     str = GetProduct(str, top_node);
// 
//     while (*str == '+' || *str == '-')
//     {
//         char op = *str;
//         ++str;
// 
//         TreeNode *buf_val = TreeNodeNew();
//         str = GetProduct(str, buf_val);
// 
//         switch (op)
//         {
//             case '+':
//                 top_node = ADD(top_node, buf_val);
//                 break;
//             case '-':
//                 top_node = SUB(top_node, buf_val);
//                 break;
//             default:
//                 assert(0 && "Syntax error\n");
//         }
//     }
// 
//     *value = *top_node;
//     free(top_node);
// 
//     return str;
// }
// 
// const char *GetProduct(const char *str, TreeNode *value)
// {
//     TreeNode *top_node = TreeNodeNew();
//     str = GetPower(str, top_node);
// 
//     while (*str == '*' || *str == '/')
//     {
//         char op = *str;
//         ++str;
// 
//         TreeNode *buf_val = TreeNodeNew();
//         str = GetPower(str, buf_val);
// 
//         switch (op)
//         {
//             case '*':
//                 top_node = MUL(top_node, buf_val);
//                 break;
//             case '/':
//                 top_node = DIV(top_node, buf_val);
//                 break;
//             default:
//                 assert(0 && "Syntax error\n");
//         }
//     }
// 
//     *value = *top_node;
//     free(top_node);
// 
//     return str;
// }
// 
// const char *GetPower(const char *str, TreeNode *value)
// {
//     TreeNode *top_node = TreeNodeNew();
//     str = GetPrimary(str, top_node);
// 
//     while (*str == '^')
//     {
//         TreeNode *buf_val = TreeNodeNew();
// 
//         ++str;
//         str = GetPrimary(str, buf_val);
// 
//         top_node = EXP(top_node, buf_val);
//     }
// 
//     *value = *top_node;
//     free(top_node);
// 
//     return str;
// }
// 
// void GetPrimary(Stack *stk, TreeNode *value)
// {
//     if (TestToken(stk, TOK_L_RND_BR))
//     {
//         NextToken(stk);
//         GetExpression(stk, value);
//         AssertToken(stk, NULL, R_FIG_BR);
//     }
//     else if (TestToken(stk, TOK_NUM))
//     {
//         GetNumber(stk, value);
//     }
//     else if (TestToken(stk, TOK_NAME))
//     {
//         Token tok = NextToken(stk);
//         if (TestToken(stk, TOK_L_RND_BR))
//         {
//             StackPush(stk, &tok);
//             GetFunction(stk, value);
//         }
//         else
//         {
//             
//         }
//     }
//     else
//     {
//         assert(0 && "GetPrimary parsing error\n");
//     }
// }
// 
// void GetNumber(Stack *stk, TreeNode *value)
// {
//     Token tok = {};
//     TokenCtor(&tok);
// 
//     AssertToken(stk, &tok, TOK_NUM);
// 
//     NUM_CTOR(value, tok.val.num);
// 
//     TokenDtor(&tok);
// 
//     return str;
// }
// 
// void GetVariable(Stack *stk, TreeNode *value)
// {
//     Token tok = {};
//     TokenCtor(&tok);
// 
//     AssertToken(stk, TOK_NAME, &tok);
// 
//     TreeNodeCtor(value,
//                  TREE_NODE_TYPE_VAR,
//                  {.var = strdup(tok.val.name)},
//                  NULL, NULL);
// 
//     TokenDtor(&tok);
// 
//     return str;
// }
// 
// void GetFunction(Stack *stk, TreeNode *value)
// {
//     int32_t op = 0;
// 
//     if (strncmp(str, "sin", 3) == 0)
//     {
//         op = OP_SIN;
//         str += 3;
//     }
//     else if (strncmp(str, "cos", 3) == 0)
//     {
//         op = OP_COS;
//         str += 3;
//     }
//     else if (strncmp(str, "ln", 2) == 0)
//     {
//         op = OP_LN;
//         str += 2;
//     }
//     else
//         assert(0 && "Wrong function");
// 
//     assert(*str == '(');
//     ++str;
// 
//     TreeNode *arg = TreeNodeNew();
//     str = GetExpression(str, arg);
// 
//     assert(*str == ')');
//     ++str;
// 
//     OP_CTOR(value, op, NULL, arg);
// 
//     return str;
// }

