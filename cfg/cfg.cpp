#include <cstdlib>
#include <sstream>
#include "cfg.h"
#include "branch_operations.h"

using namespace std;

void BasicBlock::ret(Operation *ret)
{
  ops.push_back(ret);
  
  has_return_operation = true;
}

void BasicBlock::br(BasicBlock *basic_block)
{
  if (!has_return_operation)
  {
    succs.push_back(basic_block);
  
    basic_block->preds.push_back(this);

    Operation *br = new Br(basic_block);

    ops.push_back(br);
  }
}

void BasicBlock::brc(TACMember *cond, BasicBlock *true_block, BasicBlock *false_block)
{
  if (!has_return_operation)
  {
    succs.push_back(true_block);
    succs.push_back(false_block);
    
    Operation *brc = new Brc(cond, true_block, false_block);
    
    ops.push_back(brc);
    
    true_block->preds.push_back(this);
    false_block->preds.push_back(this);
  }
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

  for(vector<Operation *>::iterator it = ops.begin(); it != ops.end(); ++it )
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


// ostream operators

ostream& operator<<(ostream& o, BasicBlock& b)
{
  return o << b.str();
}

ostream& operator<<(ostream& o, CFG& c)
{
  return o << c.str();
}

