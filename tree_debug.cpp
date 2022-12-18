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
            sprintf(value, "%.3lf", GET_NUM(CURR));
            break;
        default:
            sprintf(value, "%s", GET_VAR(CURR));
    }

    dprintf(fd, "node%ld[shape=record, label=\" {%p | %s | %s | {<left> %p | <right> %p}} \"];\n",
                idx, CURR, type_str, value, LEFT, RIGHT);

    if (LEFT)
    {
        dprintf(fd, "node%ld:<left>->node%ld;", idx, 2 * idx + 1);
        TreeDumpToFile(LEFT,  fd, 2 * idx + 1);
    }

    if (RIGHT)
    {
        dprintf(fd, "node%ld:<right>->node%ld;", idx, 2 * idx + 2);
        TreeDumpToFile(RIGHT, fd, 2 * idx + 2);
    }
}

const char *GetOperatorString(int32_t op_code)
{
    switch (op_code)
    {
        #define TYPE(name, str, cmd)            \
            case OP_TYPE_##name:                \
                return "{" #name " | " str "}";

        #include "op_types.h"

        default:
            return "(NULL)";
        #undef TYPE
    }
}

