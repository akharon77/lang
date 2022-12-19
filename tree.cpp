#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"
#include "dsl.h"

#define CURR node

void TreeDtor(TreeNode *node)
{
    ASSERT(node != NULL);

    if (LEFT)
        TreeDtor(LEFT);
    if (RIGHT)
        TreeDtor(RIGHT);

    free(CURR);
}

TreeNode *TreeNodeNew()
{
    TreeNode *res = (TreeNode*) calloc(1, sizeof(TreeNode));
    ASSERT(res != NULL);

    return res;
}

TreeNode *TreeCopy(TreeNode *node)
{
    TreeNode *node_cpy =  TreeNodeNew();
             *node_cpy = *node;

    node_cpy->left  = NULL;
    node_cpy->right = NULL;

    if (LEFT)
        node_cpy->left = TreeCopy(LEFT);
    if (RIGHT)
        node_cpy->right = TreeCopy(RIGHT);

    return node_cpy;
}

void TreeNodeAddChild(TreeNode *node, TreeNode *child)
{
    if (!node->left)
        LEFT  = child;
    else
        RIGHT = child;
}

bool NodeIsLeaf(TreeNode *node)
{
    return LEFT == NULL && RIGHT == NULL;
}

#undef CURR

void   TreeNodeCtor(TreeNode *node, int32_t type, TreeNodeValue value, TreeNode *left, TreeNode *right)
{
    *node = 
        {
            .type  = type,
            .value = value,
            .left  = left,
            .right = right 
        };
}

TreeNode *CreateTreeNode(int32_t type, TreeNodeValue val, TreeNode *left, TreeNode *right)
{
    TreeNode *node = TreeNodeNew();

    TreeNodeCtor(node, type, val, left, right);

    return node;
}

bool CompareTree(TreeNode *left, TreeNode *right)
{
    if (left == NULL && right == NULL)
        return true;
    else if (left == NULL || right == NULL)
        return false;

    return IS_EQ_NODE(left, right) && CompareTree(left->left, right->left) && CompareTree(left->right, right->right);
}

void SaveToFile(TreeNode *node, int32_t fd)
{
    dprintf(fd, "{");

    if (node != NULL)
    {
        #define TYPE(name)               \
            case TREE_NODE_TYPE_##name:  \
                dprintf(fd, #name ", "); \
                break;

        switch (GET_TYPE(node))
        {
            #include "tree_node_types.h"
        }
        #undef TYPE

        switch (GET_TYPE(node))
        {
            case TREE_NODE_TYPE_CONST:
                dprintf(fd, "%.3lf, ", GET_NUM(node));
                break;
            case TREE_NODE_TYPE_OP:
                #define TYPE(op_code, str, cmd)     \
                    case OP_TYPE_##op_code:         \
                        dprintf(fd, #op_code ", "); \
                        break;

                switch (GET_OP(node))
                {
                    #include "op_types.h"
                }
                #undef TYPE
            default:
                if (GET_VAR(node) == NULL)
                    dprintf(fd, "NULL, ");
                else
                    dprintf(fd, "%s, ", GET_VAR(node));
        }

        SaveToFile(node->left,  fd);
        dprintf(fd, ", ");
        SaveToFile(node->right, fd);
    }

    dprintf(fd, "}");
}

