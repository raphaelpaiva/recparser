#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "rm_movs.h"
#include "../cfg/cfg.h"
#include "../cfg/branch_operations.h"

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
    
      entry = replace_operations.find(attr->left->str());
    
      if (entry != replace_operations.end())
      {
        attr->left = (*entry).second;
        remove_operation = remove_move_attr_operation(op, replace_operations);
      }
      else
      {
        replace_operations[target_name] = attr->left;
        remove_operation = true;
      }
    }
    else
    {
      entry = replace_operations.find(attr->left->str());
      if (entry != replace_operations.end())
      {
        attr->left = (*entry).second;
      }
      
      entry = replace_operations.find(attr->right->str());
      if (entry != replace_operations.end())
      {
        attr->right = (*entry).second;
      }
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
    entry = replace_operations.find(ret->value->str());
    
    if (entry != replace_operations.end())
    {
      ret->value = (*entry).second;
    }
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

void remove_move_operations(CFG *cfg)
{
  map<string, TACMember *> replace_operations;
  
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    for (vector<Operation *>::iterator op = (*block)->ops.begin(); op != (*block)->ops.end();)
    {
      if (remove_move_attr_operation(*op, replace_operations))
      {
        (*block)->ops.erase(op);
        continue;
      }
      
      remove_move_return_operation(*op, replace_operations);
      remove_move_branch_operation(*op, replace_operations);
      
      ++op;
    }
  }
}

