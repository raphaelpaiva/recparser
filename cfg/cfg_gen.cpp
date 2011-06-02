#include <cstdlib>
#include <iostream>
#include "cfg.h"

extern "C" {
        #include "../driver.h"
}

using namespace std;

int main(int argc, char **argv) {
/*  DeclrListNode *declr_list;
  
  declr_list = read_ast(argc, argv);
  print_declrlist(0, declr_list); */
  
  TACVar target("t");
  TACVar left("a");
  int op = '+';
  TACVar right("b");
  
  TACOperation TACop(target, left, op, right);
  
  BasicBlock b;
  
  b << TACop;
  
  cout << b << endl;
  
  return 0;
}

