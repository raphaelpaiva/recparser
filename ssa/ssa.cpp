#include <map>
#include <iostream>
#include <algorithm>
#include <list>
#include <stack>
#include "ssa.h"
#include "../cfg/branch_operations.h"

void find_global_funcall_params(vector<TACMember *>& params, set<TACVar *, TACVarComparator>& globals, map<string, set<BasicBlock *> >& blocks, set<string>& locals);

void ssa_name_funcall_params(vector<TACMember *>& params, map<string, vector<int> >& stack);

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

void find_global_var(TACVar *var, set<TACVar *, TACVarComparator>& globals, map<string, set<BasicBlock *> >& blocks, set<string>& locals)
{
  if (var != NULL)
  {
    bool blocks_contains_var = map_contains<string, set<BasicBlock *> >(blocks, var->name);
    bool locals_contains_var = set_contains<string>(locals, var->name);
    
    if (blocks_contains_var && !locals_contains_var)
    {
      globals.insert(var);
    }
    
  }
}

void find_global_member(TACMember *member, set<TACVar *, TACVarComparator>& globals, map<string, set<BasicBlock *> >& blocks, set<string>& locals)
{
  TACVar *var = dynamic_cast<TACVar *>(member);
  
  if (var != NULL)
  {
    find_global_var(var, globals, blocks, locals);
    return;
  }
  
  TACFuncall *funcall = dynamic_cast<TACFuncall *>(member);
  
  if (funcall != NULL)
  {
    find_global_funcall_params(funcall->params, globals, blocks, locals);
    return;
  }
}

void find_global_funcall_params(vector<TACMember *>& params, set<TACVar *, TACVarComparator>& globals, map<string, set<BasicBlock *> >& blocks, set<string>& locals)
{
  for (vector<TACMember *>::iterator param = params.begin(); param != params.end(); ++param)
  {
    find_global_member(*param, globals, blocks, locals);
  }
}

void find_globals_and_add_phis(CFG *cfg)
{
  set<TACVar *, TACVarComparator> globals;
  map<string, set<BasicBlock *> > blocks;
  
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    for (set<TACVar *, TACVarComparator>::iterator var = (*block)->vars.begin(); var != (*block)->vars.end(); ++var)
    {
      if (!map_contains<string, set<BasicBlock *> >(blocks, (*var)->name))
      {
        blocks[(*var)->name];
      }
    }
  }

  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    set<string> locals;
    
    for (int i = 0; i < (*block)->ops.size(); ++i)
    {
      Operation *op = (*block)->ops[i];
      
      if (!is_type_operation<TACAttr>(op))
      {
        TACVar *var = retrieve_operand(op);
        if (var != NULL)
        {
          if (map_contains<string, set<BasicBlock *> >(blocks, var->name) && !set_contains<string>(locals, var->name))
          {
            globals.insert(var);
          }
        }
      }
      
      if(is_type_operation<TACAttr>(op))
      {
        TACAttr *attr = dynamic_cast<TACAttr *>(op);

        if (attr != NULL)
        {
          find_global_member(attr->left, globals, blocks, locals);
          find_global_member(attr->right, globals, blocks, locals);

          locals.insert(attr->target->name);
          blocks[attr->target->name].insert((*block));
        }
      }
      
      if (is_type_operation<Funcall>(op))
      {
        Funcall *funcall = dynamic_cast<Funcall *>(op);
        
        if(funcall != NULL)
        {
          find_global_funcall_params(funcall->funcall->params, globals, blocks, locals);
        }
      }
    }
  }
  
  cout << "=== globals === " << endl;
  for (set<TACVar *>::iterator var = globals.begin(); var != globals.end(); ++var)
  {
    cout << **var << endl;
  }
  
  for (set<TACVar *>::iterator var = globals.begin(); var != globals.end(); ++var)
  {
    list<BasicBlock *> work_list(blocks[(*var)->name].begin(), blocks[(*var)->name].end());
    
    while (work_list.size() > 0)
    {
      BasicBlock *block = *work_list.begin();
      
      work_list.erase(work_list.begin());
      
      for (set<BasicBlock *>::iterator frontier = block->dom_frontier.begin(); frontier != block->dom_frontier.end(); ++frontier)
      {
        if ( (*frontier)->add_phi(*var) )
        {
          work_list.push_back(*frontier);
        }
      }
    }
  }
  
}

TACVar *new_name(TACVar *var, map<string, int>& counter, map<string, vector<int> >& stack)
{
  int i = counter[var->name];
  counter[var->name]++;
  stack[var->name].push_back(i);
  
  var->index = i;
  
  cout << "new_name(): " << *var << endl;
  
  return var;
}

void ssa_name_var(TACVar *var, map<string, vector<int> >& stack)
{
  if (var != NULL)
  {
    cout << "ssa_name(): " << *var << " in " << var;
    
    if (stack[var->name].size() > 0)
    {
      var->index = stack[var->name][0];
      cout << " -> " << *var;
    }
    
    cout << endl;
  }
}

void ssa_name_member(TACMember *member, map<string, vector<int> >& stack)
{
  TACVar *var = dynamic_cast<TACVar *>(member);
  
  if (var != NULL)
  {
    ssa_name_var(var, stack);
    return;
  }
  
  TACFuncall *funcall = dynamic_cast<TACFuncall *>(member);
  
  if (funcall != NULL)
  {
    ssa_name_funcall_params(funcall->params, stack);
    return;
  }
}


void ssa_name_funcall_params(vector<TACMember *>& params, map<string, vector<int> >& stack)
{
  for (vector<TACMember *>::iterator param = params.begin(); param != params.end(); ++param)
  {
    ssa_name_member(*param, stack);
  }
}

void rename(BasicBlock *block, map<string, int>& counter, map<string, vector<int> >& stack)
{
  for (map<TACVar *, vector< pair<TACVar *, BasicBlock *> > >::iterator it = block->phis.begin(); it != block->phis.end(); ++it)
  {
    TACVar *var = (*it).first;
    vector< pair<TACVar *, BasicBlock *> > pairs = (*it).second;
    
    block->phis.erase(var);
    
    TACVar *new_var = new_name(var, counter, stack);
    
    block->phis[new_var] = pairs;
  }

  for (int i = 0; i < block->ops.size(); ++i)
  {
    Operation *op = block->ops[i];
    cout << "Parsing Operation: " << *op << " " << endl;
    if (!is_type_operation<TACAttr>(op))
    {
      TACVar *var = retrieve_operand(op);
      
      if (var != NULL)
      {
        ssa_name_var(var, stack);
      }
    }
    
    if (is_type_operation<TACAttr>(op))
    {
      TACAttr *attr = dynamic_cast<TACAttr *>(op);
      ssa_name_member(attr->left, stack);
      ssa_name_member(attr->right, stack);
        
      attr->target = new_name(attr->target, counter, stack);
    }
    
    if (is_type_operation<Funcall>(op))
      {
        Funcall *funcall = dynamic_cast<Funcall *>(op);
        
        ssa_name_funcall_params(funcall->funcall->params, stack);
      }
  }
  
  for (vector<BasicBlock *>::iterator succ = block->succs.begin(); succ != block->succs.end(); ++succ)
  {
    for (map<TACVar *, vector< pair<TACVar *, BasicBlock *> > >::iterator it = (*succ)->phis.begin(); it != (*succ)->phis.end(); ++it)
    {
      vector<pair<TACVar *, BasicBlock *> > pairs = (*it).second;
      
      for(vector<pair<TACVar *, BasicBlock *> >::iterator pair = pairs.begin(); pair != pairs.end(); ++pair)
      {
        if ((*pair).second->index == block->index)
        {
          if(stack[(*pair).first->name].size() == 0)
          {
            stack[(*pair).first->name].push_back(0);
            
            counter[(*pair).first->name]++;
          }
          
          ssa_name_var((*pair).first, stack);
        }
      }
    }
  }
  
  for(vector<BasicBlock *>::iterator child = block->children.begin(); child != block->children.end(); ++child)
  {
    rename(*child, counter, stack);
  }
  
  for (int i = 0; i < block->ops.size() - 1; ++i)
  {
    Operation *op = block->ops[i];
    if (is_type_operation<TACAttr>(op))
    {
      TACAttr *attr = dynamic_cast<TACAttr *>(op);
      
    //  cout << "attr: " << *attr << endl;
     // cout << "stack[attr->target].size() " << stack[attr->target].size() << endl;
      stack[attr->target->name].pop_back();
    }
  }
  
  for (map<TACVar *, vector< pair<TACVar *, BasicBlock *> > >::iterator it = block->phis.begin(); it != block->phis.end(); ++it)
  {
    cout << "(*it).first = " << *(*it).first << endl;
    cout << "stack[(*it).first].size() = " << stack[(*it).first->name].size() << endl;
    stack[(*it).first->name].pop_back();
  }
}

void ssa_rename(CFG *cfg)
{
  map<string, int> counter;
  map<string, vector<int> > stack;
  
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    for (set<TACVar *>::iterator var = (*block)->vars.begin(); var != (*block)->vars.end(); ++var)
    {
      if (counter.count((*var)->name) == 0)
      {
        counter[(*var)->name] = 0;
      }
      if (stack.count((*var)->name) == 0)
      {
        stack[(*var)->name];
      }
    }
  }
  
  rename(cfg->blocks[0], counter, stack);
}

void full_ssa(CFG *cfg)
{
  dom_tree(cfg);
  dom_frontier(cfg);
  find_globals_and_add_phis(cfg);
  ssa_rename(cfg);
}

/*  
  dom_tree(program.cfgs[1]);

  cout << "=== dom_tree ===" << endl;
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    cout << (*block)->name() << ": " << endl;
    for(vector<BasicBlock *>::iterator child = (*block)->children.begin(); child != (*block)->children.end(); ++child)
    {
      cout << "  " << (*child)->index << endl;
    }
  }
  
  dom_frontier(program.cfgs[1]);
  
  cout << "=== dom_frontier ===" << endl;
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    cout << (*block)->name() << ": " << endl;
    for(set<BasicBlock *>::iterator df = (*block)->dom_frontier.begin(); df != (*block)->dom_frontier.end(); ++df)
    {
      cout << "  " << (*df)->index << endl;
    }
  }
  
  cout << "=== vars ===" << endl;
  for (vector<BasicBlock *>::iterator block = cfg->blocks.begin(); block != cfg->blocks.end(); ++block)
  {
    cout << (*block)->name() << ": " << endl;
    for(set<TACVar *>::iterator var = (*block)->vars.begin(); var != (*block)->vars.end(); ++var)
    {
      cout << "  " << **var << endl;
    }
  }
*/
