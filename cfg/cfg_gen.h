#ifndef _CFG_GEN_H

#define _CFG_GEN_H

#include <cstdlib>
#include "cfg.h"

extern "C" {
  #include "../ast/ast.h"
}

using namespace std;

TACMember* gen_tac_member(Exp *ast_operation);

vector<TACOperation *> gen_expression_operations(Exp *ast_expression);

vector<TACOperation *> gen_tac_operations(CommListNode *ast_commands);

vector<BasicBlock> gen_blocks(Block *ast_block);

CFG cfg_gen(Declr *ast_declr);

vector<CFG> gen_cfgs(DeclrListNode *ast_declrs);

#endif

