#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stack_debug.h"
#include "tree.h"
#include "dsl.h"
#include "ctype.h"
#include "compiler.h"

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

void SaveToFile(TreeNode *node, int32_t fd, int32_t depth)
{
    Tabstabe(fd, depth);
    dprintf(fd, "{\n");

    if (node != NULL)
    {
        #define TYPE(name)               \
            case TREE_NODE_TYPE_##name:  \
                Tabstabe(fd, depth);     \
                dprintf(fd, #name ",\n"); \
                break;

        switch (GET_TYPE(node))
        {
            #include "tree_node_types.h"
        }
        #undef TYPE

        switch (GET_TYPE(node))
        {
            case TREE_NODE_TYPE_CONST:
                Tabstabe(fd, depth);
                dprintf(fd, "%.3lf,\n", GET_NUM(node));
                break;
            case TREE_NODE_TYPE_OP:
                #define TYPE(op_code, str, cmd)     \
                    case OP_TYPE_##op_code:         \
                        Tabstabe(fd, depth);\
                        dprintf(fd, #op_code ",\n"); \
                        break;

                switch (GET_OP(node))
                {
                    #include "op_types.h"
                }
                #undef TYPE
                break;
            default:
                if (GET_VAR(node) == NULL)
                {
                    Tabstabe(fd, depth);
                    dprintf(fd, "NULL,\n");
                }
                else
                {
                    Tabstabe(fd, depth);
                    dprintf(fd, "%s,\n", GET_VAR(node));
                }
        }

        SaveToFile(node->left,  fd, depth + 1);
        dprintf(fd, ",\n");
        SaveToFile(node->right, fd, depth + 1);
        dprintf(fd, "\n");
    }

    Tabstabe(fd, depth);
    dprintf(fd, "}");
}

const char *SkipSpaces(const char *str)
{
    while (isspace(*str))
        ++str;

    return str;
}

const char *GetTree(const char *str, TreeNode *node)
{
    str = SkipSpaces(str);
    ASSERT(*str == '{');
    ++str;
    str = SkipSpaces(str);

    if (*str == '}')
    {
        GET_TYPE(node) = -1;
        return str + 1;
    }

    #define TYPE(name)                                   \
        if (strncasecmp(str, #name, strlen(#name)) == 0) \
        {                                                \
            GET_TYPE(node) = TREE_NODE_TYPE_##name;      \
            str += strlen(#name);                        \
        }                                                \
        else
    #include "tree_node_types.h"
    // else
    {
        dprintf(2, "Unknown type here: %.10s\n", str);
        ASSERT(0);
    }
    #undef TYPE

    str = SkipSpaces(str);
    ASSERT(*str == ',');
    ++str;
    str = SkipSpaces(str);

    switch (GET_TYPE(node))
    {
        case TREE_NODE_TYPE_CONST:
            {
                int32_t offset = 0;
                sscanf(str, "%lf%n", &GET_NUM(node), &offset);
                str += offset;
            }
            break;
        case TREE_NODE_TYPE_NVAR:
        case TREE_NODE_TYPE_NFUN:
        case TREE_NODE_TYPE_CALL:
        case TREE_NODE_TYPE_VAR:
        case TREE_NODE_TYPE_ARG:
        case TREE_NODE_TYPE_ASS:
            {
                int32_t len           = 0;
                const char *str_old = str;

                while (*str != ',')
                    ++str;

                GET_VAR(node) = strndup(str_old, str - str_old);
            }
            break;
        case TREE_NODE_TYPE_OP:
            {
                #define TYPE(op_code, tr1, tr2)                             \
                    if (strncasecmp(str, #op_code, strlen(#op_code)) == 0)  \
                    {                                                       \
                        GET_OP(node) = OP_TYPE_##op_code;                   \
                        str += strlen(#op_code);                            \
                    }                                                       \
                    else

                #include "op_types.h"
                #undef TYPE
                // else
                {
                    dprintf(2, "Unknown operation code here: %.10s\n", str);
                    ASSERT(0);
                }
            }
            break;
        default:
            if (strncasecmp(str, "NULL", 4) == 0)
            {
                GET_VAR(node) = NULL;
                str += 4;
            }
            else
                ASSERT(0);
    }

    str = SkipSpaces(str);
    ASSERT(*str == ',');
    ++str;
    str = SkipSpaces(str);

    node->left = TreeNodeNew();
    str = GetTree(str, node->left);
    if (GET_TYPE(node->left) == -1)
    {
        free(node->left);
        node->left = NULL;
    }

    str = SkipSpaces(str);
    ASSERT(*str == ',');
    ++str;
    str = SkipSpaces(str);

    node->right = TreeNodeNew();
    str = GetTree(str, node->right);
    if (GET_TYPE(node->right) == -1)
    {
        free(node->right);
        node->right = NULL;
    }

    str = SkipSpaces(str);
    ASSERT(*str == '}');
    ++str;

    return str;
}

