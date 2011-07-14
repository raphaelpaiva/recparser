#include <map>
#include <iostream>
#include <algorithm>
#include "ssa.h"
#include "../cfg/branch_operations.h"

template<class T, class U, class Comparator>
bool map_contains(map<T, U, Comparator> m, T t)
{
  return m.find(t) != m.end();
}

template<class T, class Comparator>
bool set_contains(set<T, Comparator> s, T t)
{
  return s.find(t) != s.end();
}

bool compare_basic_block_by_index(BasicBlock *b1, BasicBlock *b2)
{
  return b1->index < b2->index;
}

void bfs_visit(BasicBlock *block, map<int, bool> &marks, vector<int> &n, vector<int> &rpo)
{
  marks[block->index] = true;
  for (vector<BasicBlock *>::iterator child = block->succs.begin(); child != block->succs.end(); ++child)
  {
    if (!map_contains<int, bool>(marks, (*child)->index))
    {
      bfs_visit(*child, marks, n, rpo);
    }
  }
  
  block->rpo = n[0];
  rpo[block->rpo] = block->index;
  n[0] -= 1;
}

vector<int> rpo(CFG *cfg)
{
  map<int, bool> marks;
  vector<int> n;
  vector<int> rpo(cfg->blocks.size(), 0);
  
  n.push_back(cfg->blocks.size() - 1);
  
  bfs_visit(cfg->blocks[0], marks, n, rpo);
  
  return rpo;
}

BasicBlock *intersect(BasicBlock *left, BasicBlock *right)
{
  BasicBlock *finger1 = left;
  BasicBlock *finger2 = right;
  
  while (finger1->index != finger2->index)
  {
    while (finger1->rpo > finger2->rpo)
    {
      finger1 = finger1->idom;
    }
    while(finger2->rpo > finger1->rpo)
    {
      finger2 = finger2->idom;
    }
  }
  
  return finger1;
}

void dom_tree(CFG* cfg)
{
  vector<int> nodes_rpo = rpo(cfg);
  cfg->blocks[0]->idom = cfg->blocks[0];
  
  for (int i = 1; i < cfg->blocks.size(); i++)
  {
    int block_index = nodes_rpo[i];
    BasicBlock *block = cfg->blocks[block_index];
    
    if (block->preds.size() == 2 && block->preds[1]->idom != NULL)
    {
      block->idom = intersect(block->preds[0], block->preds[1]);
      block->idom->children.push_back(block);
    }
    else
    {
      block->idom = block->preds[0];
      block->idom->children.push_back(block);
    }
  }
  
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    sort((*block)->children.begin(), (*block)->children.end(), compare_basic_block_by_index);
  }
}

void dom_frontier(CFG *cfg)
{
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    if ((*block)->preds.size() > 1)
    {
      for(vector<BasicBlock *>::iterator pred = (*block)->preds.begin(); pred != (*block)->preds.end(); ++pred)
      {
        BasicBlock *runner = *pred;
        while (runner->index != (*block)->idom->index)
        {
          runner->dom_frontier.insert(*block);
          runner = runner->idom;
        }
      }
    }
  }
}

template<class T>
T *is_type_operation(Operation *op)
{
  return dynamic_cast<T *>(op);
}

TACVar *retrieve_operand(Operation *op)
{
  if (is_type_operation<Return>(op))
  {
    Return *ret = dynamic_cast<Return *>(op);
    return dynamic_cast<TACVar *>(ret->value);
  }
  
  if (is_type_operation<Brc>(op))
  {
    Brc *brc = dynamic_cast<Brc *>(op);
    return dynamic_cast<TACVar *>(brc->cond);
  }
  
  return NULL;
}

/*
  def find_globals(nodes):
    blocks = {}
    for n in nodes:
        for var in n.vars:
            if not var in blocks:
                blocks[var] = set()
    globals = set()
    for n in nodes:
        locals = set()
        for i in range(len(n.ops)-1):
            op = n.ops[i]
            if op[0] != "=" and op[3] in blocks and op[3] not in locals:
                globals.add(op[3])
            if op[2] in blocks and op[2] not in locals:
                globals.add(op[2])
            locals.add(op[1])
            blocks[op[1]].add(n)
        jump = n.ops[-1]
        if jump != "br":
            if jump[1] in blocks and jump[1] not in locals:
                globals.add(jump[1])
    return (globals, blocks)
*/


//op[1] = dest; op[2] = left; op[3] = right;
void find_globals(CFG *cfg)
{
  set<TACVar *, TACVarComparator> globals;
  map<TACVar *, set<BasicBlock *>, TACVarComparator> blocks;
  
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    for (set<TACVar *>::iterator var = (*block)->vars.begin(); var != (*block)->vars.end(); ++var)
    {
      if (!map_contains<TACVar *, set<BasicBlock *> >(blocks, (*var)))
      {
        blocks[(*var)];
        cout << **var << " in blocks " << map_contains<TACVar *, set<BasicBlock *> >(blocks, *var) << endl; 
      }
    }
  }

  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    set<TACVar *, TACVarComparator> locals;
    
    for (int i = 0; i < (*block)->ops.size(); ++i)
    {
      Operation *op = (*block)->ops[i];
      
      if (!is_type_operation<TACAttr>(op))
      {
        TACVar *var = retrieve_operand(op);
        if (var != NULL)
        {
          if (map_contains<TACVar *, set<BasicBlock *>, TACVarComparator>(blocks, var) && !set_contains<TACVar *, TACVarComparator>(locals, var))
          {
            globals.insert(var);
            blocks[var].insert((*block));
          }
        }
      }
      else if(is_type_operation<TACAttr>(op))
      {
        TACAttr *attr = dynamic_cast<TACAttr *>(op);
        if (attr != NULL)
        {
          cout << "OE! attr_operation: " << *attr << " in block " << (*block)->index << endl;
          
          TACVar *left = dynamic_cast<TACVar *>(attr->left);
          TACVar *right = dynamic_cast<TACVar *>(attr->right);
          TACVar *target = attr->target;

          if ( (left != NULL) && (map_contains<TACVar *, set<BasicBlock *>, TACVarComparator>(blocks, left) &&
                                !set_contains<TACVar *, TACVarComparator>(locals, left)) )
          {
            cout << ">> inserindo global: " << *left << *block << endl;
            globals.insert(left);
          }
          
          if ( (right != NULL) && (map_contains<TACVar *, set<BasicBlock *>, TACVarComparator>(blocks, right) &&
                                !set_contains<TACVar *, TACVarComparator>(locals, right)) )
          {
            cout << ">> inserindo global: " << *left << *block << endl;
            globals.insert(right);
          }
          
          cout << ">> inserindo local: " << *attr->target << " em " << (*block)->index << endl;
          locals.insert(attr->target);
          blocks[attr->target].insert((*block));
        }
      }
      if (is_type_operation<TACFuncall>(op))
      {
        TACFuncall *funcall = dynamic_cast<TACFuncall *>(op);
        
        if(funcall != NULL)
        {
          cout << "OE! TACFuncall_operation: " << *funcall << " in block " << (*block)->index << endl;
          for (vector<TACMember *>::iterator param = funcall->params.begin(); param != funcall->params.end(); ++param)
          {
            TACVar *var_param = dynamic_cast<TACVar *>(*param);
      
            if ( (var_param != NULL) && (map_contains<TACVar *, set<BasicBlock *>, TACVarComparator>(blocks, var_param) &&
                                         !set_contains<TACVar *, TACVarComparator>(locals, var_param)) )
            {
              cout << ">> inserindo global: " << *left << *block << endl;
              globals.insert(var_param);
            }
          }
        }
      }
    }
  }
  
/*  cout << "=== locals === " << endl;
  for (set<TACVar *>::iterator var = locals.begin(); var != locals.end(); ++var)
  {
    cout << (*var) << endl;
  }
  */
  cout << "=== globals === " << endl;
  for (set<TACVar *>::iterator var = globals.begin(); var != globals.end(); ++var)
  {
    cout << **var << endl;
  }
}
