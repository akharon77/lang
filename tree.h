#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include "tree_struct.h"

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
    OP_TYPE_EQ,
    OP_TYPE_EXP,
    OP_TYPE_NEQ,
    OP_TYPE_MOD,
    N_OP_TYPES
};

#define TYPE(name) TREE_NODE_TYPE_##name,
enum TREE_NODE_TYPES
{
    #include "tree_node_types.h"
    N_TREE_NODE_TYPES
};
#undef TYPE

void      TreeCtor         (TreeNode *node);
void      TreeDtor         (TreeNode *node);

TreeNode *TreeNodeNew      ();
void      TreeNodeCtor     (TreeNode *node, int32_t type, TreeNodeValue value, TreeNode *left, TreeNode *right);

TreeNode *CreateTreeNode   (int32_t type, TreeNodeValue val, TreeNode *left, TreeNode *right);
TreeNode *TreeCopy         (TreeNode *node);

bool      TreeNodeIsLeaf   (TreeNode *node);
void      TreeNodeAddChild (TreeNode *node, TreeNode *child);
TreeNode *GetMaxSubtree    (TreeNode *node);

bool CompareTree(TreeNode *left, TreeNode *right);
bool NodeIsLeaf(TreeNode *node);
#endif  // TREE_H

