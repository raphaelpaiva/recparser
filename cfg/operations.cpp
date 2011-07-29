#include <string>
#include <sstream>
#include "operations.h"

string Operation::str()
{
    return "Generic Operation";
}

string TACAttr::str()
{
  stringstream ss;
  
  ss << *target << " = ";
  
  if (op != 0)
  {
    ss << retrieve_operation_string(op) << " " << *left << ", " << *right;
  }
  else
  {
    ss << *left;
  }
  
  return ss.str();
}


string Return::str()
{
  stringstream ss;
  
  ss << "ret i32 ";
  
  if (value != NULL)
  {
    ss << *value;
  }
  
  return ss.str();
}

string Load::str()
{
  stringstream ss;
  
  ss << *target << " <- load " << *value;
  
  return ss.str();
}

string Funcall::str()
{
  stringstream ss;
  
  ss << "call " << *funcall;
  
  return ss.str();
}

ostream& operator<<(ostream& o, Operation& op)
{
  return o << op.str();
}

ostream& operator<<(ostream& o, TACAttr& a)
{
  return o << a.str();
}

