#include <map>
#include <iostream>
#include "ssa.h"

/*
 def rpo(nodes):
    marks = {}
    n = [len(nodes)-1]
    rpo = [0] * len(nodes)
    def bfs_visit(node):
        marks[node] = True
        for child in node.succs:
            if not marks.get(child, False):
                bfs_visit(child)
        node.rpo = n[0]
        rpo[node.rpo] = node
        n[0] -= 1
    bfs_visit(nodes[0])
    return rpo
*/

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


