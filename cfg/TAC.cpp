#include <vector>
#include <sstream>
#include "TAC.h"

using namespace std;

template<>
string Literal<string>::str() {
  stringstream ss;
  
  ss << "\"" << value << "\"";
  
  return ss.str();
}

string TACMember::str()
{
  return name;
}

string TACVar::str()
{
  stringstream ss;
  
  ss << "%" << name << "." << index;
  
  return ss.str();
}

string TACFuncall::str()
{
  if (params.size() == 0)
  {
    return "call i32 " + name + "()";
  }
  
  stringstream ss;
  
  ss << "call i32 " << name << '(';
  
  for (vector<TACMember *>::iterator it = params.begin(); it != params.end(); ++it)
  {
    ss << "i32 " << (*it)->str() << ", ";
  }
  
  string partial = ss.str();
  

  partial = partial.substr(0, partial.length() - 2);
  
  partial += ")";
  
  return partial;
}

ostream& operator<<(ostream& o, TACMember& v)
{
  return o << v.str();
}

