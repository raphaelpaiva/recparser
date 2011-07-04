#include <cstdlib>
#include <iostream>
#include <vector>
#include "cfg/cfg_gen.h"

extern "C" {
        #include "parser/driver.h"
}


using namespace std;

int main(int argc, char **argv)
{
  DeclrListNode *ast;
  
  vector<CFG> cfgs;

  ast = read_ast(argc, argv);
  print_declrlist(0, ast);
  
  cfgs = gen_cfgs(ast);
  
  for (vector<CFG>::iterator it = cfgs.begin(); it != cfgs.end(); ++it)
  {
    cout << *it << endl;
  }

  return 0;
}

