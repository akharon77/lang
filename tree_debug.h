#ifndef DIFFR_DEBUG_H
#define DIFFR_DEBUG_H

#include "tree_struct.h"

void        TreeDump          (TreeNode *node, const char *filename);
void        TreeDumpToFile    (TreeNode *node, int32_t fd, int64_t idx);
const char *GetOperatorString (int32_t op_code);

#endif  // DIFFR_DEBUG_H

