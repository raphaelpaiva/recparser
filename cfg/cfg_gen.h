#ifndef _CFG_GEN_H

#define _CFG_GEN_H

#include <cstdlib>
#include "cfg.h"

extern "C" {
  #include "../ast/ast.h"
}

using namespace std;

Prog gen_prog(DeclrListNode *ast_declrs);

#endif

