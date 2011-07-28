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

  Prog program = gen_prog(ast);
  
  for (vector<CFG *>::iterator cfg = program.cfgs.begin(); cfg != program.cfgs.end(); ++cfg)
  {
    full_ssa(*cfg);
  }
  
  cout << program;
  
  return 0;
}

