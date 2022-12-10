#ifndef TREE_STRUCT_H
#define TREE_STRUCT_H

union TreeNodeValue
{
    int64_t  dbl;
    int32_t  op;
    char    *var;
};

struct TreeNode
{
    int32_t type;

    TreeNodeValue value;

    TreeNode *left;
    TreeNode *right;
};

#endif  // TREE_STRUCT_H

