#ifndef _CFG_H

#define _CFG_H

#include <string>
#include <sstream>
#include <vector>
#include "TAC.h"
#include "operations.h"

using namespace std;

static int last_index = 0;

class BasicBlock {
  public:
    int index;
    bool has_return_operation;
    
    vector<Operation *> ops;
    vector<BasicBlock *> succs;
    vector<BasicBlock *> preds;
    
    BasicBlock() : index(last_index), has_return_operation(false) { last_index++; };

    friend ostream& operator<<(ostream& o, BasicBlock& b);
    
    string name();
    string str(int);
    string str();
    
    void br(BasicBlock *basic_block);
    void brc(TACMember *cond, BasicBlock *true_block, BasicBlock *false_block);
    void ret(Operation *ret);
};

class CFG {
  public:
    string name;
    vector<BasicBlock *> blocks;
    BasicBlock *work_block;
    
    CFG() {};
    CFG(string paramName) : name(paramName) {};
    CFG(string paramName, vector<BasicBlock *> paramBlocks) : name(paramName), blocks(paramBlocks) {};
    
    friend ostream& operator<<(ostream& o, CFG& c);
    
    string str();
};

class Prog {
  public:
    vector<CFG *> cfgs;
    vector<TACVar *> globals;
    vector<TACAttr *>global_attrs;
    
    Prog() {};
    
    friend ostream& operator<<(ostream& o, Prog& prog);
    
    string str();
};

#endif

