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
      ss << "(COMMAND_IF)";
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

string retrieve_expression_tag(int tag)
{
  stringstream ss;
  
  ss << tag;
  
  switch(tag)
  {
    case EXP_INT: {
      ss << " (EXP_INT)";
      break;
    }
    case EXP_FLOAT: {
      ss << "(EXP_FLOAT)";
      break;
    }
    case EXP_STRING: {
      ss << "(EXP_STRING)";
      break;
    }
    case EXP_VAR: {
      ss << "(EXP_VAR)";
      break;
    }
    case EXP_BINOP: {
      ss << "(EXP_BINOP)";
      break;
    }
    case EXP_NEG: {
      ss << "(EXP_NEG)";
    }
    case EXP_LNEG: {
      ss << "(EXP_LNEG)";
    }
    case EXP_FUNCALL: {
      ss << "(EXP_FUNCALL)";
    }
    case EXP_CONV: {
      ss << "(EXP_CONV)";
    }
    default: {
      break;
    }
  }
  
  return ss.str();
}

void error(string message)
{
  cout << "ERROR: " << message << endl;
  exit(1);
}

ostream& operator<<(ostream& o, Command& ast_command)
{
  return o << retrieve_command_tag(ast_command.tag) << " in line " << ast_command.line << endl;
}

void error(string message, Command *ast_command)
{
  cout << "ERROR: " <<  message << endl
       << "Command: " << *ast_command << endl;
  exit(1);
}

ostream& operator<<(ostream& o, Exp& ast_expression)
{
  return o << retrieve_expression_tag(ast_expression.tag) << " in line " << ast_expression.line << endl;
}

void error(string message, Exp *ast_expression)
{
  cout << "ERROR: " <<  message << endl
       << "Expression: " << *ast_expression << endl;
  exit(1);
}

bool is_attr_command(Command *ast_command)
{
  return ast_command != NULL && ast_command->tag == COMMAND_ATTR;
}

