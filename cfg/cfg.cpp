#include <cstdlib>
#include <sstream>
#include <algorithm>
#include "cfg.h"
#include "branch_operations.h"

using namespace std;

bool BasicBlock::add_phi(TACVar *var)
{
  if (phis.count(var) == 0)
  {
    vector<pair<TACMember *, BasicBlock *> > pairs;
    
    for (vector<BasicBlock *>::iterator block = preds.begin(); block != preds.end(); ++block)
    {
      pair<TACMember *, BasicBlock *> pair(new TACVar(var), *block);
      
      pairs.push_back(pair);
    }
    
    phis[var] = pairs;
    
    return true;
  }
  
  return false;
}

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
  if (ops.empty())
  {
    return "";
  }

  stringstream ss;
  string spaces;
  
  for (int i = 0; i < indent + 2; i++)
  {
    spaces += " ";
  }
  
  ss << "  " << name() << ":" << endl;
  
  for (map<TACVar *, vector<pair<TACMember *, BasicBlock *> >, TACVarComparator>::iterator phis_member = phis.begin(); phis_member != phis.end(); ++phis_member)
  {
    ss << spaces << *(*phis_member).first << " = " << "phi i32 ";
    
    vector<pair<TACMember *, BasicBlock *> > phis_vector = (*phis_member).second;
    
    for (vector<pair<TACMember *, BasicBlock *> >::iterator phi = phis_vector.begin(); phi != phis_vector.end(); ++phi )
    {
      ss << "[ " << *(*phi).first << ", " <<  "%" << (*phi).second->name() << " ], "; 
    }
    
    long pos = ss.tellp();
    
    ss.seekp(pos - 2);
    
    ss << endl;
  }
  
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
  
  ss << "define i32 @" << name << " {" << endl;
  
  for(vector<BasicBlock *>::iterator it = blocks.begin(); it != blocks.end(); ++it )
  {
    ss << (*it)->str(2) << endl;
  }
  
  ss << "}";
  
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

ostream& operator<<(ostream& o, Prog& prog)
{
  return o << prog.str();
}

