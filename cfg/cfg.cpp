#include <stdio.h>
#include <stdlib.h>

extern "C" {
        #include "../driver.h"
}

int main(int argc, char **argv) {
  FILE *f;
  DeclrListNode *declr_list;
  
  f = openFile(argc, argv);
  
  startLex(f);
  
  declr_list = declrs(0, 0);
  print_declrlist(0, declr_list);
  
  
  printf("hello world!!\n");
  return 0;
}
