#define TOKEN(name, type, cmp, str, op_code, cmd) TYPE (op_code, str, cmd)
TYPE (NEG, "-", "")
#include "tokens.h"
#undef TOKEN

