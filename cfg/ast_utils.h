#ifndef _AST_UTILS_H

#define _AST_UTILS_H

#include <cstdlib>
#include <sstream>
#include <string>

extern "C" {
  #include "../ast/ast.h"
}

using namespace std;

string retrieve_command_tag(int tag);
string retrieve_expression_tag(int tag);
void error(string message);
ostream& operator<<(ostream& o, Command& ast_command);
void error(string message, Command *ast_command);
ostream& operator<<(ostream& o, Exp& ast_command);
void error(string message, Exp *ast_expression);
bool is_attr_command(Command *ast_command);

#endif
