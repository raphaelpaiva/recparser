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
  
  r = rpo(program.cfgs[0]);
  
  for (vector<int>::iterator it = r.begin(); it != r.end(); ++it)
  {
    cout << *it << endl;
  }
  
  return 0;
}

