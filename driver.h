#ifndef DRIVER_H

#define DRIVER_H

#include "decl.h"
#include "ast.h"

#define SYNTAX_ERROR(message, args...) printf("Sintax error in line %i: ", yylineno); printf(message, ## args); puts("\n"); exit(0);
#define NO_BINOP -1

int yylex(void);

extern int yylineno;
extern FILE *outfile;
extern char *filename;

int token;

CommListNode *commandl();
Exp *simple();
Block *block();
Declr *declr(DeclrListNode *root, int from_block, int from_function);
DeclrListNode *declrs(int from_block, int from_function);

FILE *open_file(int, char **);
void start_lex(FILE *);
void startup(int , char **);
DeclrListNode *read_ast(int argc, char **argv);

#endif
