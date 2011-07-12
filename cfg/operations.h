#ifndef _OPERATIONS_H

#define _OPERATIONS_H

#include <string>
#include "TAC.h"
#include "ast_utils.h"

class Operation {
	public:
		virtual string str();
		friend ostream& operator<<(ostream& o, Operation& op);
};

class Return : public Operation {
  public:
    TACMember *value;

    Return() : value(NULL) {};
    Return(TACMember *paramValue) : value(paramValue) {};
    string str();
};

class TACAttr : public Operation {
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

class Load : public Operation {
  public:
    TACMember *target;
    TACMember *value;
    
    Load() {};
    Load(TACMember *paramTarget, TACMember *paramValue) : target(paramTarget), value(paramValue) {};
    
    string str();
};

class Funcall : public Operation {
  public:
    TACFuncall *funcall;
    
    Funcall() {};
    Funcall(TACFuncall *paramFuncall) : funcall(paramFuncall) {};
    
    string str();
};

#endif

