#include <cstdlib>
#include <iostream>
#include "cfg.h"

extern "C" {
        #include "../driver.h"
}

using namespace std;

CFG cfg_gen(Declr *declr)
{
  CFG cfg(declr->u.name);
  
  return cfg;
}

int main(int argc, char **argv) {
  DeclrListNode *declrs;
  
  vector<CFG> cfgs;
  
  declrs = read_ast(argc, argv);
  print_declrlist(0, declrs);

  while(declrs != NULL)
  {
    if (declrs->declr->tag == DECLR_FUNC)
    {
      cfgs.push_back(cfg_gen(declrs->declr));
    }
    
    declrs = declrs->next;
  }
  
  for (vector<CFG>::iterator it = cfgs.begin(); it != cfgs.end(); ++it)
  {
    cout << *it << endl;
  }

  return 0;
}

/*  
  TACVar target("t");
  TACVar left("a");
  int op = '+';
  TACVar right("b");
  
  TACOperation TACop(target, left, op, right);
  
  BasicBlock b;
  
  b << TACop;
  
  CFG c("main");
  
  c << b;
  
  cout << c << endl;
  */
