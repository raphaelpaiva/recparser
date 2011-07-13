#include <map>
#include <iostream>
#include <algorithm>
#include "ssa.h"

bool compare_basic_block_by_index(BasicBlock *b1, BasicBlock *b2)
{
  return b1->index < b2->index;
}

void bfs_visit(BasicBlock *block, map<int, bool> &marks, vector<int> &n, vector<int> &rpo)
{
  marks[block->index] = true;
  for (vector<BasicBlock *>::iterator child = block->succs.begin(); child != block->succs.end(); ++child)
  {
    bool child_is_not_marked = marks.find((*child)->index) == marks.end();
    if (child_is_not_marked)
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

