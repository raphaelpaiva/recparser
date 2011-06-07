#include <cstdlib>
#include <sstream>
#include "cfg.h"

using namespace std;

string TACMember::str()
{
  return name;
}

string TACVar::str()
{
  stringstream ss;
  
  ss << name << "_" << index;
  
  return ss.str();
}

string TACLiteral::str()
{
  stringstream ss;
  
  ss << value;
  
  return ss.str();
}

string TACFuncall::str()
{
  stringstream ss;
  
  ss << name << '(';
  
  for (vector<TACMember>::iterator it = params.begin(); it != params.end(); ++it)
  {
    ss << (*it).str() << ", ";
  }
  
  string partial = ss.str();
  
  partial = partial.substr(0, partial.length() - 1);
  
  partial += ")";
  
  return partial;
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
  
  ss << "B" << index << ":";
  
  return ss.str();
}

string BasicBlock::str(int indent)
{
  stringstream ss;
  string spaces;
  
  for (int i = 0; i < indent + 2; i++)
  {
    spaces += " ";
  }
  
  ss << "  " << name() << endl;

  for(vector<TACOperation>::iterator it = ops.begin(); it != ops.end(); ++it )
  {
    ss << spaces << *it << endl;
  }
  
  return ss.str();
}

string BasicBlock::str()
{
  return str(0);
}

void BasicBlock::operator<<(TACOperation& op)
{
  ops.push_back(op);
}

void CFG::operator<<(BasicBlock& b)
{
  blocks.push_back(b);
}

string CFG::str()
{
  stringstream ss;
  
  ss << name << ":" << endl;
  
  for(vector<BasicBlock>::iterator it = blocks.begin(); it != blocks.end(); ++it )
  {
    ss << (*it).str(2) << endl;
  }
  
  return ss.str();
}

// ostream operators
ostream& operator<<(ostream& o, TACMember& v)
{
  return o << v.str();
}
/*
ostream& operator<<(ostream& o, TACMember& v)
{
  return o << v.str();
}

ostream& operator<<(ostream& o, TACMember& v)
{
  return o << v.str();
}
*/
ostream& operator<<(ostream& o, TACOperation& op)
{
  return o << op.str();
}

ostream& operator<<(ostream& o, BasicBlock& b)
{
  return o << b.str();
}

ostream& operator<<(ostream& o, CFG& c)
{
  return o << c.str();
}
