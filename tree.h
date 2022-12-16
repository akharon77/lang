#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include "tree_struct.h"

#define TYPE(name, str, cmd) OP_TYPE_##name,
enum OP_TYPES
{
    #include "op_types.h"
    N_OP_TYPES
};
#undef TYPE

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

bool      CompareTree      (TreeNode *left, TreeNode *right);
bool      NodeIsLeaf       (TreeNode *node);
#endif  // TREE_H

