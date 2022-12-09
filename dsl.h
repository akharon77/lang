#ifndef DSL_H
#define DSL_H

#define NUM_CTOR(node, val)         TreeNodeCtor   (node,     TREE_NODE_TYPE_NUM, {.dbl = val},         NULL, NULL)
#define CREATE_NUM(val)             CreateTreeNode (          TREE_NODE_TYPE_NUM, {.dbl = val},         NULL, NULL)

#define OP_CTOR(node, val, lhs, rhs) TreeNodeCtor   (node,     TREE_NODE_TYPE_OP,  {.op  = val},          lhs,  rhs)

#define VAR_CTOR(node, val, subst)         TreeNodeCtor   (node,     TREE_NODE_TYPE_VAR, {.var = strdup(val)}, NULL, subst)
#define CREATE_VAR(val)             CreateTreeNode (          TREE_NODE_TYPE_VAR, {.var = strdup(val)}, NULL, NULL)

#define LEFT                        (CURR)->left
#define RIGHT                       (CURR)->right

#define D_L                         Differentiate (LEFT, LGR)
#define D_R                         Differentiate (RIGHT, LGR)

#define CP_L                        TreeCopy (LEFT)
#define CP_R                        TreeCopy (RIGHT)
#define CP_CR                       TreeCopy (CURR)

#define ADD(lhs, rhs)               CreateTreeNode (TREE_NODE_TYPE_OP, {.op = OP_TYPE_ADD}, lhs, rhs)
#define SUB(lhs, rhs)               CreateTreeNode (TREE_NODE_TYPE_OP, {.op = OP_TYPE_SUB}, lhs, rhs)

#define MUL(lhs, rhs)               CreateTreeNode (TREE_NODE_TYPE_OP, {.op = OP_TYPE_MUL}, lhs, rhs)
#define DIV(lhs, rhs)               CreateTreeNode (TREE_NODE_TYPE_OP, {.op = OP_TYPE_DIV}, lhs, rhs)

#define EXP(lhs, rhs)               CreateTreeNode (TREE_NODE_TYPE_OP, {.op = OP_TYPE_EXP}, lhs,  rhs)

#define IS_OP(node)                 (node != NULL && GET_TYPE(node) == TREE_NODE_TYPE_OP)
#define IS_OP_CODE(node, op_code)   (IS_OP(node) && GET_OP(node) == op_code)

#define IS_NUM(node)                (node != NULL && GET_TYPE(node) == TREE_NODE_TYPE_NUM)

#define EPS                          1e-6

#define IS_EQ(node, val)            (IS_NUM(node)                && \
                                     (val) - EPS < GET_NUM(node) && \
                                     GET_NUM(node) < (val) + EPS)

#define IS_EQ_NODE(lhs, rhs)        (IS_NUM(lhs) && IS_NUM(rhs) && IS_EQ(lhs, GET_NUM(rhs)) ||  \
                                     IS_VAR(lhs) && IS_VAR(rhs) &&                              \
                                     strcasecmp(GET_VAR(lhs), GET_VAR(rhs)) == 0            ||  \
                                     IS_OP(lhs) && IS_OP(rhs) && IS_OP_CODE(lhs, GET_OP(rhs)))

#define IS_ZERO(node)               (IS_EQ(node, 0))
#define IS_ONE(node)                (IS_EQ(node, 1))

#define IS_VAR(node)                (node != NULL && GET_TYPE(node) == TREE_NODE_TYPE_VAR)

#define IS_FUNC(node)               (node != NULL && (IS_OP(node) || IS_VAR(node)))

#define GET_TYPE(node)              ((node)->type)
#define GET_NUM(node)               ((node)->value.dbl)
#define GET_OP(node)                ((node)->value.op)
#define GET_VAR(node)               ((node)->value.var)

#endif  // DSL_H
