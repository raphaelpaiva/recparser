#ifndef _CFG_H

#define _CFG_H

#include <string>
#include <sstream>
#include <vector>

using namespace std;

class TACOperation {
	public:
		virtual string str();
		friend ostream& operator<<(ostream& o, TACOperation& op);
};

class TACMember {
  public:
    string name;
  
    TACMember() {};
    TACMember(string paramName) : name(paramName) {};
    
    friend ostream& operator<<(ostream& o, TACMember& m);
    
    virtual string str();
};

class TACVar : public TACMember {
  public:
    int index;
    
    TACVar(string paramName) :  TACMember(paramName), index(0) { };
    TACVar(string paramName, int temp_index) :  index(0) { stringstream ss; ss << paramName << temp_index; name = ss.str(); };
    
    string str();
};

template<class T>
class TACLiteral : public TACMember {
  public:
    T value;
    
    TACLiteral() {};
    
    TACLiteral(T paramValue) : value(paramValue) {};
    
    string str()
    {
      stringstream ss;
  
      ss << value;

      return ss.str();
    }
    
};

class TACFuncall : public TACMember, public TACOperation {
  public:
    vector<TACMember *> params;
  
    TACFuncall() {};
    TACFuncall(string paramName) : TACMember(paramName) {};
    TACFuncall(string paramName, vector<TACMember *> paramParams) : TACMember(paramName), params(paramParams) {};
    
    string str();
};

class TACReturn : public TACOperation {
  public:
    TACMember *value;

    TACReturn() : value(NULL) {};
    TACReturn(TACMember *paramValue) : value(paramValue) {};
    string str();
};

class TACAttr : public TACOperation {
  public:
    TACMember *target;
    TACMember *left;
    TACMember *right;
    int op;
    
    TACAttr() {};
    TACAttr(TACMember *paramTarget, TACMember *paramLeft, int paramOp, TACMember *paramRight) : target(paramTarget), left(paramLeft), right(paramRight), op(paramOp) {} ;
    TACAttr(TACMember *paramTarget, TACMember *paramLeft) : target(paramTarget), left(paramLeft), op(0), right(NULL) {} ;
    friend ostream& operator<<(ostream& o, TACAttr& op);
    
    string str();
};

class Load : public TACOperation {
  public:
    TACMember *target;
    TACMember *value;
    
    Load() {};
    Load(TACMember *paramTarget, TACMember *paramValue) : target(paramTarget), value(paramValue) {};
    
    string str();
};

static int last_index = 0;

class BasicBlock {
  public:
    int index;
    vector<TACOperation *> ops;
    
    BasicBlock() : index(last_index) { last_index++; };

    friend ostream& operator<<(ostream& o, BasicBlock& b);
    
    string name();
    string str(int);
    string str();
};

class CFG {
  public:
    string name;
    BasicBlock *block;
    
    CFG() {};
    CFG(string paramName) : name(paramName) {};
    CFG(string paramName, BasicBlock * paramBlock) : name(paramName), block(paramBlock) {};
    
    friend ostream& operator<<(ostream& o, CFG& c);
    
    string str();
};

class Prog {
  public:
    vector<CFG *> cfgs;
    vector<TACVar *> globals;
    vector<TACAttr *>global_attrs;
    
    Prog() {};
    
    string str();
};


#endif
