#include <iostream>

extern "C" {
        #include "../driver.h"
}

using namespace std;

int main(int argc, char **argv) {
  FILE *f;
  DeclrListNode *declr_list;
  
  f = open_file(argc, argv);
  
  start_lex(f);
  
  declr_list = declrs(0, 0);
  print_declrlist(0, declr_list);
  
  
  
  return 0;
}
