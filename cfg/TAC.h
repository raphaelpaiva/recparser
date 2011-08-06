#ifndef _TAC_H

#define _TAC_H

#include <vector>
#include <string>
#include <sstream>

using namespace std;

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
    TACVar(string paramName, int temp_index) :  index(0) { 
      stringstream ss;
      ss << paramName << temp_index;
      name = ss.str();
    };
    TACVar(TACVar *old_var) : TACMember(old_var->name), index(0) { };
    
    string str();
};

struct TACVarComparator {
    bool operator() (TACVar const *v1, TACVar const *v2) const {
      return v1->name < v2->name;
    }
};

template<class T>
class Literal : public TACMember {
  public:
    T value;
    
    Literal() {};
    
    Literal(T paramValue) : value(paramValue) {};
    
    string str()
    {
      stringstream ss;
  
      ss << value;

      return ss.str();
    }
    
};

class TACFuncall : public TACMember {
  public:
    vector<TACMember *> params;
  
    TACFuncall() {};
    TACFuncall(string paramName) : TACMember(paramName) {};
    TACFuncall(string paramName, vector<TACMember *> paramParams) : TACMember(paramName), params(paramParams) {};
    
    string str();
};

#endif
