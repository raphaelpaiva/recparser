#include <cstdlib>
#include <sstream>
#include "cfg.h"

using namespace std;

string TACVar::str()
{
  stringstream ss;
  
  ss << name << "_" << index;
  
  return ss.str();
}

string TACOperation::str()
{
  stringstream ss;
  
  ss << target << " <- " << left << " " << (char) op << " " << right;
  
  return ss.str();
}

string BasicBlock::name()
{
  stringstream ss;
  
  ss << "B:" << index;
  
  return ss.str();
}

string BasicBlock::str()
{
  stringstream ss;

  ss << name() << endl;

  for(vector<TACOperation>::iterator it = ops.begin(); it != ops.end(); ++it )
  {
    ss << "  "<< *it << endl;
  }
  
  return ss.str();
}

void BasicBlock::operator<<(TACOperation& op)
{
  ops.push_back(op);
}


// ostream operators
ostream& operator<<(ostream& o, TACVar& v)
{
  return o << v.str();
}

ostream& operator<<(ostream& o, TACOperation& op)
{
  return o << op.str();
}

ostream& operator<<(ostream& o, BasicBlock& b)
{
  return o << b.str();
}

