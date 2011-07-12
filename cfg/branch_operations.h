#ifndef _BRANCH_OPERATIONS_H

#define _BRANCH_OPERATIONS_H

#include "operations.h"
#include "cfg.h"

class Br : public Operation {
  public:
    BasicBlock *basic_block;
    
    Br() {};
    Br(BasicBlock *paramBasicBlock) : basic_block(paramBasicBlock) {};
    
    string str();
};

class Brc : public Operation {
  public:
    TACMember *cond;
    BasicBlock *true_block;
    BasicBlock *false_block;
    
    Brc() {};
    Brc(TACMember *paramCond, BasicBlock *paramTrue_block, BasicBlock *paramFalse_block) : cond(paramCond), true_block(paramTrue_block), false_block(paramFalse_block) {};
    
    string str();
};

#endif
