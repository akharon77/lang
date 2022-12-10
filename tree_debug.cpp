#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "tree.h"
#include "stack_debug.h"
#include "tree_debug.h"
#include "dsl.h"

void TreeDump(TreeNode *node, const char *filename)
{
    ASSERT(node != NULL);

    static int32_t cnt = 0;

    char filename_txt[512] = "",
         filename_svg[512] = "";

    sprintf(filename_txt, "dump/%s_%d.txt", filename, cnt);
    sprintf(filename_svg, "dump/%s_%d.svg", filename, cnt++);

    int32_t fd = creat(filename_txt, S_IRWXU);
    ASSERT(fd != -1);

    dprintf(fd, "digraph G {\n");
    TreeDumpToFile(node, fd, 0);
    dprintf(fd, "}\n");

    close(fd);

    char cmd[512] = "";
    sprintf(cmd, "dot %s -o %s -Tsvg", filename_txt, filename_svg);
    system(cmd);
}

#define CURR node

void TreeDumpToFile(TreeNode *node, int32_t fd, int64_t idx)
{
    const char *type_str   = "";
          char  value[256] = "";

#define TYPE(name)              \
    case TREE_NODE_TYPE_##name: \
        type_str = #name;       \
        break;

    switch (GET_TYPE(CURR))
    {
        #include "tree_node_types.h"
    }
#undef TYPE

    switch (GET_TYPE(CURR))
    {
        case TREE_NODE_TYPE_OP:
            sprintf(value, "%s", GetOperatorString(GET_OP(CURR)));
            break;
        case TREE_NODE_TYPE_NUM:
            sprintf(value, "%ld", GET_NUM(CURR));
            break;
        case TREE_NODE_TYPE_NVAR:
        case TREE_NODE_TYPE_VAR:
        case TREE_NODE_TYPE_NFUN:
        case TREE_NODE_TYPE_FUNC:
        case TREE_NODE_TYPE_PAR:
            sprintf(value, "%s", GET_VAR(CURR));
            break;
        defalut:
            sprintf(value, "(NULL)");
    }

    dprintf(fd, "node%ld[shape=record, label=\" { %s | %s } \"];\n",
                idx, type_str, value);

    if (LEFT)
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 1);
        TreeDumpToFile(LEFT,  fd, 2 * idx + 1);
    }

    if (RIGHT)
    {
        dprintf(fd, "node%ld->node%ld;", idx, 2 * idx + 2);
        TreeDumpToFile(RIGHT, fd, 2 * idx + 2);
    }
}

const char *GetOperatorString(int32_t op_code)
{
    switch (op_code)
    {
        case OP_TYPE_ADD:
            return "{ADD | +}";
        case OP_TYPE_SUB:
            return "{SUB | -}";
        case OP_TYPE_MUL:
            return "{MUL | *}";
        case OP_TYPE_DIV:
            return "{DIV | /}";
        case OP_TYPE_EXP:
            return "{EXP | ^}";
        case OP_TYPE_AND:
            return "{AND | and}";
        case OP_TYPE_OR:
            return "{OR | or}";
        case OP_TYPE_NOT:
            return "{NOT | !}";
        case OP_TYPE_ASS:
            return "{ASS | =}";
        case OP_TYPE_LES:
            return "{LES | <}";
        case OP_TYPE_LEQ:
            return "{LEQ | <=}";
        case OP_TYPE_GER:
            return "{GER | >}";
        case OP_TYPE_GEQ:
            return "{GEQ | >=}";
        case OP_TYPE_EQ:
            return "{EQ | ==}";
        case OP_TYPE_NEQ:
            return "{NEQ | !=}";
        case OP_TYPE_MOD:
            return "{MOD | %}";
        default:
            return "(NULL)";
    }
}

