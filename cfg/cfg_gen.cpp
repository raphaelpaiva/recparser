#include <cstdlib>
#include <iostream>
#include "cfg_gen.h"
#include "ast_utils.h"

using namespace std;

TACMember *gen_operations(TACVar *target, Exp *ast_expression, BasicBlock *basic_block)
{
  switch(ast_expression->tag)
  {
    case EXP_INT: {
      TACMember *literal = new TACLiteral(ast_expression->u.ival);
      
      if (target != NULL)
      {
        TACOperation *operation = new TACAttr(target, literal);
        basic_block->ops.push_back(operation);
      }
      
      return literal;
      break;
    }
    case EXP_VAR: {
      TACMember *var = new TACVar(ast_expression->u.var->name);
      
      if (target != NULL)
      {
        TACOperation *operation = new TACAttr(target, var);
        basic_block->ops.push_back(operation);
      }
      
      return var;
      break;
    }
    case EXP_BINOP: {
      TACMember *left = gen_operations(NULL, ast_expression->u.binop.e1, basic_block);
      TACMember *right = gen_operations(NULL, ast_expression->u.binop.e2, basic_block);
      int op = ast_expression->u.binop.op;
      
      if (target == NULL)
      {
        target = new TACVar("t");
      }
      
      TACOperation *operation = new TACAttr(target, left, op, right);
      
      basic_block->ops.push_back(operation);
      
      return target;
      
      break;
    }
    case EXP_STRING: {
      error("String Expression Handling not implemented yet!");
      break;
    }
    
    default: {
      error("Unhandled expression type", ast_expression);
      break;
    }
  }
}

vector<BasicBlock *> gen_commands(Block *ast_block)
{
  vector<BasicBlock *> blocks;
  CommListNode *ast_commands;
  
  ast_commands = ast_block->comms;
  
  while (ast_commands != NULL)
  {
    Command *ast_command;
    ast_command = ast_commands->comm;
    
    switch(ast_command->tag)
    {
      case COMMAND_ATTR: {
        BasicBlock *basic_block = new BasicBlock();
        
        TACVar *target = new TACVar(ast_command->u.attr.lvalue->name);

        gen_operations(target, ast_command->u.attr.rvalue, basic_block);

        blocks.push_back(basic_block);
        
        break;
      }
      case COMMAND_RET: {
        
        break;
      }
      default: {
        error("Unhandled command type.", ast_command);
        break;
      }
    }
    
    ast_commands = ast_commands->next;
  }
  
  return blocks;
}

CFG gen_cfg(Declr *ast_declr)
{
  CFG cfg(ast_declr->u.name);
  
  cfg.blocks = gen_commands(ast_declr->u.func.block);
  
  return cfg;
}

vector<CFG> gen_cfgs(DeclrListNode *ast_declrs)
{
  vector<CFG> cfgs;

  while(ast_declrs != NULL)
  {
    if (ast_declrs->declr->tag == DECLR_FUNC)
    {
      CFG cfg = gen_cfg(ast_declrs->declr);
      cfgs.push_back(cfg);
    }
    
    ast_declrs = ast_declrs->next;
  }
  
  return cfgs;
}

