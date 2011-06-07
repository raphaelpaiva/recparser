#ifndef CFG_H

#define CFG_H

#include <string>
#include <vector>

using namespace std;

class TACMember {
  public:
    string name;
  
    TACMember() {};
    TACMember(string paramName) : name(paramName) {};
    
    friend ostream& operator<<(ostream& o, TACMember& m);
    
    string str();
};

class TACVar : public TACMember {
  public:
    int index;
    
    TACVar(string paramName) :  TACMember(paramName), index(0) { };
    
    string str();
};

class TACLiteral : public TACMember {
  public:
    int value;
    
    TACLiteral() {};
    
    TACLiteral(int paramValue) : value(paramValue) {};
    
    string str();
};

class TACFuncall : public TACMember {
  public:
    vector<TACMember> params;
  
    TACFuncall() {};
    TACFuncall(string paramName) : TACMember(paramName) {};
    TACFuncall(string paramName, vector<TACMember> paramParams) : TACMember(paramName), params(paramParams) {};
    
    string str();
};

class TACOperation {
  public:
    TACMember target;
    TACMember left;
    TACMember right;
    int op;
    
    TACOperation() {};
    TACOperation(TACMember paramTarget, TACMember paramLeft, int paramOp, TACMember paramRight) : target(paramTarget), left(paramLeft), right(paramRight), op(paramOp) {} ;
    
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
