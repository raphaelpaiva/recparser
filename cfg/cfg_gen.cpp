#include <cstdlib>
#include <iostream>
#include "cfg.h"

extern "C" {
        #include "../driver.h"
}

using namespace std;

void error(string message)
{
  cout << "ERROR! " << message << endl;
  exit(0);
}

bool is_attr_command(Command *ast_command)
{
  return ast_command != NULL && ast_command->tag == COMMAND_ATTR;
}

TACMember gen_tac_member(Exp *ast_operation)
{
  switch (ast_operation->tag) {
    case EXP_INT: {
      TACLiteral literal(ast_operation->u.ival);
      cout << "literal: " << literal.str() << endl;
      return literal;
      break;
    }
    case EXP_VAR: {
      TACVar var(ast_operation->u.var->name);
      return var;
      break;
    }
    case EXP_FUNCALL: {
      TACFuncall funcall(ast_operation->u.funcall.name);
      return funcall;
      break;
    }
    default: {
      error("Tipo de membro de expressao não tratado!");
      break;
    }
  }
}

vector<TACOperation> gen_expression_operations(Exp *ast_expression)
{
  vector<TACOperation> ops;
  
  switch (ast_expression->tag) {
    case EXP_BINOP: {
      TACMember target("t");
      TACMember left = gen_tac_member(ast_expression->u.binop.e1);
      TACMember right = gen_tac_member(ast_expression->u.binop.e2);
      int op = ast_expression->u.binop.op;
      
      TACOperation tac_operation(target, left, op, right);
      
      ops.push_back(tac_operation);
      
      break;
    }
    default: {
      error("expressao de tipo não tratado.");
      break;
    }
  }
    
  return ops;
}

vector<TACOperation> gen_tac_operations(CommListNode *ast_commands)
{
  Command *ast_command;
  vector<TACOperation> ops;
  
  ast_command = ast_commands->comm;
  
  while (ast_commands != NULL && is_attr_command(ast_command))
  {
    TACMember target(ast_command->u.attr.lvalue->name);
    
    vector<TACOperation> exp_ops = gen_expression_operations(ast_command->u.attr.rvalue);
    
    ops.insert(ops.end(), exp_ops.begin(), exp_ops.end());
    
    ast_commands = ast_commands->next;
    ast_command = ast_commands->comm;
  }
  
  return ops;
}

vector<BasicBlock> gen_blocks(Block *ast_block)
{
  vector<BasicBlock> blocks;
  CommListNode *ast_commands;
  
  ast_commands = ast_block->comms;
  
  while (ast_commands != NULL)
  {
    Command *ast_command;
    ast_command = ast_commands->comm;
    
    if (ast_command->tag == COMMAND_ATTR)
    {
      BasicBlock block;
      
      block.ops = gen_tac_operations(ast_commands);
      
      blocks.push_back(block);
    }
    
    ast_commands = ast_commands->next;
  }
  
  return blocks;
}

CFG cfg_gen(Declr *ast_declr)
{
  CFG cfg(ast_declr->u.name);
  
  cfg.blocks = gen_blocks(ast_declr->u.func.block);
  
  return cfg;
}

int main(int argc, char **argv) {
  DeclrListNode *ast_declrs;
  
  vector<CFG> cfgs;

  ast_declrs = read_ast(argc, argv);
  print_declrlist(0, ast_declrs);
  
  while(ast_declrs != NULL)
  {
    if (ast_declrs->declr->tag == DECLR_FUNC)
    {
      CFG cfg = cfg_gen(ast_declrs->declr);
      cfgs.push_back(cfg);
    }
    
    ast_declrs = ast_declrs->next;
  }
  
  for (vector<CFG>::iterator it = cfgs.begin(); it != cfgs.end(); ++it)
  {
    cout << *it << endl;
  }

  return 0;
}

/*  
  TACVar target("t");
  TACVar left("a");
  int op = '+';
  TACVar right("b");
  
  TACOperation TACop(target, left, op, right);
  
  BasicBlock b;
  
  b << TACop;
  
  CFG c("main");
  
  c << b;
  
  cout << c << endl;
  */
