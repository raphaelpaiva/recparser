#ifndef CFG_H

#define CFG_H

#include <string>
#include <vector>

using namespace std;

class TACVar {
  public:
    string name;
    int index;
  
    TACVar() {};
    TACVar(string paramName) : name(paramName), index(0) { };
    
    friend ostream& operator<<(ostream& o, TACVar& v);
    
    string str();
};

class TACOperation {
  public:
    TACVar target;
    TACVar left;
    TACVar right;
    int op;
    
    TACOperation() {};
    TACOperation(TACVar paramTarget, TACVar paramLeft, int paramOp, TACVar paramRight) : target(paramTarget), left(paramLeft), right(paramRight), op(paramOp) {} ;
    
    friend ostream& operator<<(ostream& o, TACOperation& op);
    
    string str();
};

static int last_index = 0;

class BasicBlock {
  public:
    int index;
    vector<TACOperation> ops;
    
    BasicBlock() : index(last_index) { last_index++; };

    friend ostream& operator<<(ostream& o, BasicBlock& b);
    void operator<<(TACOperation& b);
    
    string name();
    string str(int);
    string str();
};

class CFG {
  public:
    string name;
    vector<BasicBlock> blocks;
    
    CFG() {};
    CFG(string paramName) : name(paramName) {};
    
    void operator<<(BasicBlock& b);
    
    friend ostream& operator<<(ostream& o, CFG& c);
    
    string str();
};

#endif
