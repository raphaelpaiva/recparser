#ifndef AST_UTILS_H

#define AST_UTILS_H

#include <cstdlib>
#include <sstream>
#include <string>

extern "C" {
  #include "../ast.h"
}

using namespace std;

string retrieve_command_tag(int tag);
ostream& operator<<(ostream& o, Command& ast_command);
void error(string message);
void error(string message, Command *ast_command);
bool is_attr_command(Command *ast_command);

#endif
