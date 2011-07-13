#include <cstdlib>
#include <iostream>
#include <vector>
#include "cfg/cfg_gen.h"

extern "C" {
        #include "parser/driver.h"
        #include "parser/type_checker.h"
}


using namespace std;

int main(int argc, char **argv)
{
  DeclrListNode *ast;
  
  vector<CFG *> cfgs;

  ast = read_ast(argc, argv);

  check_prog(ast);

  print_declrlist(0, ast);
  
  Prog program = gen_prog(ast);
  
  cout << program; //TODO: cout << program Y U NO WORK?
  
  return 0;
}

