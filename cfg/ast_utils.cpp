#include <iostream>
#include <sstream>
#include <string>
#include "ast_utils.h"

using namespace std;

string retrieve_command_tag(int tag)
{
  stringstream ss;
  
  ss << tag;
  
  switch(tag)
  {
    case COMMAND_IF: {
      ss << " (COMMAND_IF))";
      break;
    }
    case COMMAND_WHILE: {
      ss << "(COMMAND_WHILE)";
      break;
    }
    case COMMAND_ATTR: {
      ss << "(COMMAND_ATTR)";
      break;
    }
    case COMMAND_RET : {
      ss << "(COMMAND_RET)";
      break;
    }
    case COMMAND_FUNCALL: {
      ss << "(COMMAND_FUNCALL)";
      break;
    }
    case COMMAND_BLOCK: {
      ss << "(COMMAND_BLOCK)";
    }    
    default: {
      break;
    }
  }
  
  return ss.str();
  
}

ostream& operator<<(ostream& o, Command& ast_command)
{
  return o << retrieve_command_tag(ast_command.tag) << " in line " << ast_command.line << endl;
}

void error(string message)
{
  cout << "ERROR: " << message << endl;
  exit(1);
}

void error(string message, Command *ast_command)
{
  cout << "ERROR: " <<  message << endl
       << "Command: " << *ast_command << endl;
  exit(1);
}

bool is_attr_command(Command *ast_command)
{
  return ast_command != NULL && ast_command->tag == COMMAND_ATTR;
}

