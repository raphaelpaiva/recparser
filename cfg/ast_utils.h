#ifndef _AST_UTILS_H

#define _AST_UTILS_H

#include <cstdlib>
#include <sstream>
#include <string>

extern "C" {
  #include "../ast/ast.h"
}

using namespace std;

string retrieve_operation_string(int op);
bool can_short_circuit(int op);
void error(string message);
void error(string message, Command *ast_command);
void error(string message, Exp *ast_expression);

#endif
