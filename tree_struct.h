#ifndef TREE_STRUCT_H
#define TREE_STRUCT_H

enum OP_TYPES
{
    OP_TYPE_ADD,
    OP_TYPE_SUB,
    OP_TYPE_MUL,
    OP_TYPE_DIV,
    OP_TYPE_AND,
    OP_TYPE_OR,
    OP_TYPE_NOT,
    OP_TYPE_ASS,
    OP_TYPE_LES,
    OP_TYPE_LEQ,
    OP_TYPE_GER,
    OP_TYPE_GEQ,
    OP_TYPE_XOR,
    OP_TYPE_EQ,
    OP_TYPE_EXP,
    OP_TYPE_NEQ,
    OP_TYPE_DEN,
    OP_TYPE_PRCNT,
    N_OP_TYPES
};

enum TREE_NODE_TYPES
{
    TREE_NODE_TYPE_NUM,
    TREE_NODE_TYPE_VAR,
    TREE_NODE_TYPE_OP,
    TREE_NODE_TYPE_CALL,
    TREE_NODE_TYPE_ARG,
    TREE_NODE_TYPE_DEFS,
    TREE_NODE_TYPE_IF,
    TREE_NODE_TYPE_WHILE,
    TREE_NODE_TYPE_BLOCK,
    TREE_NODE_TYPE_NVAR,
    TREE_NODE_TYPE_NFUN,
    TREE_NODE_TYPE_RET,
    TREE_NODE_TYPE_STMNT,  // TODO
    TREE_NODE_TYPE_DEF,
    TREE_NODE_TYPE_PAR,
    TREE_NODE_TYPE_ASS,
    N_TREE_NODE_TYPES
};

union
{
    int64_t  dbl;
    int32_t  op;
    char    *var;
} TreeNodeValue;

struct TreeNode
{
    int32_t type;

    TreeNodeValue value;

    int32_t size;

    TreeNode *left;
    TreeNode *right;
};

#endif  // TREE_STRUCT_H

