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

string TACFuncall::str()
{
  if (params.size() == 0)
  {
    return "()";
  }
  
  stringstream ss;
  
  ss << name << '(';
  
  for (vector<TACMember *>::iterator it = params.begin(); it != params.end(); ++it)
  {
    ss << (*it)->str() << ", ";
  }
  
  string partial = ss.str();
  

  partial = partial.substr(0, partial.length() - 2);
  
  partial += ")";
  
  return partial;
}

string TACOperation::str()
{
    return "Generic Operation";
}

string TACAttr::str()
{
  stringstream ss;
  
  ss << *target << " <- " << *left;
  
  if (op != 0)
  {
    ss << " " << (char) op << " " << *right;
  }
  
  return ss.str();
}

string Br::str()
{
  stringstream ss;
  
  ss << "br " << basic_block->name();
  
  return ss.str();
}

void BasicBlock::br(BasicBlock *basic_block)
{
  succs.push_back(basic_block);
  
  basic_block->preds.push_back(this);
  
  TACOperation *br = new Br(basic_block);
  
  ops.push_back(br);
}

void BasicBlock::brc(TACMember *cond, BasicBlock *true_block, BasicBlock *false_block)
{
  succs.push_back(true_block);
  succs.push_back(false_block);
  
  TACOperation *brc = new Brc(cond, true_block, false_block);
  
  ops.push_back(brc);
  
  true_block->preds.push_back(this);
  false_block->preds.push_back(this);
}

string BasicBlock::name()
{
  stringstream ss;
  
  ss << "B" << index;
  
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
  
  ss << "  " << name() << ":" << endl;

  for(vector<TACOperation *>::iterator it = ops.begin(); it != ops.end(); ++it )
  {
    ss << spaces << **it << endl;
  }
  
  return ss.str();
}

string BasicBlock::str()
{
  return str(0);
}

string CFG::str()
{
  stringstream ss;
  
  ss << name << ":" << endl;
  
  for(vector<BasicBlock *>::iterator it = blocks.begin(); it != blocks.end(); ++it )
  {
    ss << (*it)->str(2) << endl;
  }
  
  return ss.str();
}

string TACReturn::str()
{
  stringstream ss;
  
  ss << "ret ";
  
  if (value != NULL)
  {
    ss << *value;
  }
  
  return ss.str();
}

string Prog::str()
{
  stringstream ss;
  
  for (vector<TACAttr *>::iterator it = global_attrs.begin(); it != global_attrs.end(); ++it)
  {
    ss << **it << endl;
  }
  
  for (vector<CFG *>::iterator it = cfgs.begin(); it != cfgs.end(); ++it)
  {
    ss << **it << endl;
  }
  
  return ss.str();
}

string Load::str()
{
  stringstream ss;
  
  ss << *target << " <- load " << *value;
  
  return ss.str();
}

string Brc::str()
{
  stringstream ss;
  
  ss << "brc " << *cond << " " << true_block->name() << " " << false_block->name();
  
  return ss.str();
}

// ostream operators
ostream& operator<<(ostream& o, TACMember& v)
{
  return o << v.str();
}

ostream& operator<<(ostream& o, TACOperation& op)
{
  return o << op.str();
}

ostream& operator<<(ostream& o, TACAttr& op)
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

ostream& operator<<(ostream& o, TACReturn& ret)
{
  return o << ret.str();
}

ostream& operator<<(ostream& o, Prog& prog)
{
  return o << prog.str();
}

ostream& operator<<(ostream& o, Load& load)
{
  return o << load.str();
}

ostream& operator<<(ostream& o, Br& br)
{
  return o << br.str();
}

ostream& operator<<(ostream& o, Brc& brc)
{
  return o << brc.str();
}
