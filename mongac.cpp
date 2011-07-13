#include <cstdlib>
#include <iostream>
#include <vector>
#include "cfg/cfg_gen.h"
#include "ssa/ssa.h"

extern "C" {
        #include "parser/driver.h"
        #include "parser/type_checker.h"
}


using namespace std;

int main(int argc, char **argv)
{
  DeclrListNode *ast;
  vector<int> r;
  
  ast = read_ast(argc, argv);

  check_prog(ast);

  print_declrlist(0, ast);
  
  Prog program = gen_prog(ast);
  
  cout << program.str();
  
  dom_tree(program.cfgs[0]);

  cout << "=== dom_tree ===" << endl;
  for (vector<BasicBlock *>::iterator block = program.cfgs[0]->blocks.begin(); block != program.cfgs[0]->blocks.end(); ++block)
  {
    cout << (*block)->name() << ": " << endl;
    for(vector<BasicBlock *>::iterator child = (*block)->children.begin(); child != (*block)->children.end(); ++child)
    {
      cout << "  " << (*child)->index << endl;
    }
  }
  
  dom_frontier(program.cfgs[0]);
  
  cout << "=== dom_frontier ===" << endl;
  for (vector<BasicBlock *>::iterator block = program.cfgs[0]->blocks.begin(); block != program.cfgs[0]->blocks.end(); ++block)
  {
    cout << (*block)->name() << ": " << endl;
    for(set<BasicBlock *>::iterator df = (*block)->dom_frontier.begin(); df != (*block)->dom_frontier.end(); ++df)
    {
      cout << "  " << (*df)->index << endl;
    }
  }
  
  return 0;
}

