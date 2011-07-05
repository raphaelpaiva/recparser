#include <cstdlib>
#include <iostream>
#include "cfg_gen.h"
#include "ast_utils.h"

using namespace std;

TACVar *gen_temp()
{
  return new TACVar("t", last_temp_index++);
}

TACMember *gen_operations(TACVar *target, Exp *ast_expression, BasicBlock *basic_block)
{
  if (ast_expression == NULL) {
    return NULL;
  }
  
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
      stringstream ss;
      
      ss << ast_expression->u.var->name << "_" << ast_expression->u.var->var;
      
      TACMember *var = new TACVar(ss.str());
      
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
        target = gen_temp();
      }
      
      TACOperation *operation = new TACAttr(target, left, op, right);
      
      basic_block->ops.push_back(operation);
      
      return target;
      
      break;
    }
    case EXP_FUNCALL: {
      string name = ast_expression->u.funcall.name;
      ExpListNode *ast_params = ast_expression->u.funcall.expl;

      vector<TACMember *> params;

      while (ast_params != NULL)
      {
        Exp *param_exp = ast_params->exp;
        
        TACMember *param = gen_operations(NULL, param_exp, basic_block);
        
        params.push_back(param);
        
        ast_params = ast_params->next;
      }
      
      TACMember *funcall = new TACFuncall(name, params);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACOperation *attr = new TACAttr(target, funcall);
      
      basic_block->ops.push_back(attr);
      
      return target;
      break;
    }
    case EXP_NEG : {
      TACMember *neg = gen_operations(NULL, ast_expression->u.exp, basic_block);
      
      TACMember *zero = new TACLiteral(0);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACOperation *attr = new TACAttr(target, zero, '-', neg);
      
      basic_block->ops.push_back(attr);
      
      return target;
      break;
    }
    case EXP_STRING: {
      error("String Expression Handling not implemented yet!");
      break;
    }
    case EXP_CONV: {
      error("CONV Expression Handling not implemented yet!");
      break;
    }
    default: {
      error("Unhandled expression type", ast_expression);
      break;
    }
  }
}

TACOperation *gen_return_operation(Exp *ast_expression, BasicBlock *basic_block)
{
  TACMember *ret_value = gen_operations(NULL, ast_expression, basic_block);
  TACOperation *ret = new TACReturn(ret_value);
  
  return ret;
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
    
    BasicBlock *basic_block = new BasicBlock();
    
    switch(ast_command->tag)
    {
      case COMMAND_ATTR: {
        TACVar *target = new TACVar(ast_command->u.attr.lvalue->name);

        gen_operations(target, ast_command->u.attr.rvalue, basic_block);

        break;
      }
      case COMMAND_RET: {
        TACOperation *ret;
        
        ret = gen_return_operation(ast_command->u.ret, basic_block);
        
        basic_block->ops.push_back(ret);
        
        break;
      }
      default: {
        error("Unhandled command type.", ast_command);
        break;
      }
    }
    
    blocks.push_back(basic_block);
    
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
    last_index = 0;
  }
  
  return cfgs;
}

