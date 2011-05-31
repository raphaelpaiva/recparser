#include <stdio.h>
#include <stdlib.h>

extern "C" {
        #include "../lex.yy.c"
        #include "../driver.h"
}

int main(int argc, char **argv) {
  FILE *f;
  DeclrListNode *declr_list;
 
  if(argc > 1) {
    f = fopen(argv[1], "r");
    filename = argv[1];
  } else {
    f = stdin;
    filename = "stdin";
  }
 
  if(!f) {
    fprintf(stderr, "Cannot open file %s. Exiting...", filename);
    exit(0);
  }
 
  yyrestart(f);
  yylineno = 1;
  outfile = stdout;
  filename = "stdout";
  token = yylex();
  
  declr_list = declrs(0, 0);
  print_declrlist(0, declr_list);
  
  return 0;
}
