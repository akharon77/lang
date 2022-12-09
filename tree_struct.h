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
    N_OP_TYPES
};

enum TREE_NODE_TYPES
{
    TREE_NODE_TYPE_NUM,
    TREE_NODE_TYPE_VAR,
    TREE_NODE_TYPE_OP,
    TREE_NODE_TYPE_CALL,
    TREE_NODE_TYPE_ARG,
    TREE_NODE_TYPE_SEQ,
    TREE_NODE_TYPE_IF,
    TREE_NODE_TYPE_WHILE,
    TREE_NODE_TYPE_BLOCK,
    TREE_NODE_TYPE_NVAR,
    TREE_NODE_TYPE_DEF,
    TREE_NODE_TYPE_PAR,
    N_TREE_NODE_TYPES
};

union
{
    double  dbl;
    int32_t op;
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

