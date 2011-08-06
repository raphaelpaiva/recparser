#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "rm_movs.h"
#include "../cfg/cfg.h"
#include "../cfg/branch_operations.h"

vector<TACMember *> remove_move_funcall_params(vector<TACMember *> params, map<string, TACMember *>& replace_operations)
{
  vector<TACMember *> new_params;
  
  for (vector<TACMember *>::iterator param = params.begin(); param != params.end(); ++param)
  {
    map<string, TACMember *>::iterator entry;
  
    entry = replace_operations.find( (*param)->str() );

    if (entry != replace_operations.end())
    {
      new_params.push_back((*entry).second);
    }
    else
    {
      new_params.push_back(*param);
    }
  }
  
  return new_params;
}

bool remove_move_member(TACMember **member, map<string, TACMember *>& replace_operations)
{
  if (member != NULL)
  {
    TACFuncall *funcall = dynamic_cast<TACFuncall *>(*member);
  
    if (funcall != NULL)
    {
      funcall->params = remove_move_funcall_params(funcall->params, replace_operations);
    }
    else
    {
      map<string, TACMember *>::iterator entry;
      entry = replace_operations.find((*member)->str());
      
      if (entry != replace_operations.end())
      {
        *member = (*entry).second;
        return true;
      }
    }
  }
  
  return false;
}

void remove_move_funcall_operation(Operation *op, map<string, TACMember *>& replace_operations)
{
  Funcall *funcall = dynamic_cast<Funcall *>(op);
  
  if (funcall != NULL)
  {
    TACFuncall *tac_funcall = funcall->funcall;
    
    tac_funcall->params = remove_move_funcall_params(tac_funcall->params, replace_operations);
  }
}

bool remove_move_attr_operation(Operation *op, map<string, TACMember *>& replace_operations)
{
  bool remove_operation = false;
  
  TACAttr *attr = dynamic_cast<TACAttr *>(op);
  map<string, TACMember *>::iterator entry;
  
  if (attr != NULL)
  {
    if (attr->right == NULL)
    {
      string target_name = attr->target->str();
    
      if (remove_move_member(&(attr->left), replace_operations))
      {
        remove_operation = remove_move_attr_operation(op, replace_operations);
      }
      else
      {
        TACFuncall *funcall = dynamic_cast<TACFuncall *>(attr->left);
        
        if (funcall == NULL)
        {
          replace_operations[target_name] = attr->left;
          remove_operation = true;
        }
      }
    }
    else
    {
      remove_move_member(&(attr->left), replace_operations);
      
      remove_move_member(&(attr->right), replace_operations);
    }
  }
  
  return remove_operation;
}

bool remove_move_return_operation(Operation *op, map<string, TACMember *>& replace_operations)
{
  Return *ret = dynamic_cast<Return *>(op);
  map<string, TACMember *>::iterator entry;
  
  if (ret != NULL)
  {
    remove_move_member(&(ret->value), replace_operations);
  }
}

void remove_move_branch_operation(Operation *op, map<string, TACMember *>& replace_operations)
{
  Brc *brc = dynamic_cast<Brc *>(op);
  map<string, TACMember *>::iterator entry;
  
  if (brc != NULL)
  {
    entry = replace_operations.find(brc->cond->str());
    
    if (entry != replace_operations.end())
    {
      brc->cond = (*entry).second;
    }
  }
}

void remove_phi_operations(BasicBlock *block, map<string, TACMember *>& replace_operations)
{
  map<TACVar *, vector< pair<TACMember *, BasicBlock *> >, TACVarComparator > new_phis;
  
  for (vector<BasicBlock *>::iterator succ = block->succs.begin(); succ != block->succs.end(); ++succ)
  {
    for (map<TACVar *, vector< pair<TACMember *, BasicBlock *> > >::iterator it = (*succ)->phis.begin(); it != (*succ)->phis.end(); ++it)
    {
      vector<pair<TACMember *, BasicBlock *> > new_pairs;
      
      vector<pair<TACMember *, BasicBlock *> > pairs = (*it).second;
      
      for(vector<pair<TACMember *, BasicBlock *> >::iterator pair = pairs.begin(); pair != pairs.end(); ++pair)
      {
        map<string, TACMember *>::iterator entry;
        
        TACMember *var = (*pair).first;
        
        entry = replace_operations.find(var->str());
        
        if (entry != replace_operations.end())
        {
          new_pairs.push_back(make_pair((*entry).second, (*pair).second));
        }
        else
        {
          new_pairs.push_back(*pair);
        }
      }
      
      new_phis[(*it).first] = new_pairs;
    }
    
    (*succ)->phis = new_phis;
  }

}

void remove_move_operation(BasicBlock *block, map<string, TACMember *> &replace_operations) {
    for (vector<Operation *>::iterator op = block->ops.begin(); op != block->ops.end();)
    {
      if (remove_move_attr_operation(*op, replace_operations))
      {
        block->ops.erase(op);
        continue;
      }
      
      remove_move_return_operation(*op, replace_operations);
      remove_move_branch_operation(*op, replace_operations);
      remove_move_funcall_operation(*op, replace_operations);
      
      ++op;
    }
    
    remove_phi_operations(block, replace_operations);

    for(vector<BasicBlock *>::iterator child = block->children.begin(); child != block->children.end(); ++child)
    {
      remove_move_operation(*child, replace_operations);
    }
}

void remove_move_operations(CFG *cfg)
{
  map<string, TACMember *> replace_operations;
  remove_move_operation(cfg->blocks[0], replace_operations);
}

