#include <iostream>
#include <cfg.h>

extern "C" {
        #include "../driver.h"
}

using namespace std;

int main(int argc, char **argv) {
  DeclrListNode *declr_list;
  
  declr_list = read_ast(argc, argv);
  print_declrlist(0, declr_list);
  
  return 0;
}
