#include <cstdlib>
#include <iostream>
#include "cfg_gen.h"
#include "ast_utils.h"

using namespace std;

TACMember *gen_operations(TACVar *target, Exp *ast_expression, CFG *cfg);
TACMember *gen_attr_binop(TACVar *target, Exp *ast_expression, CFG *cfg);

static int last_temp_index;
static int last_global_index;

Prog prog;

TACVar *gen_var(Var *ast_var)
{
  stringstream ss;
  
  ss << ast_var->name << "_" << ast_var->var;
  
  return new TACVar(ss.str());
}

bool can_short_circuit(int op)
{
  return op == TK_AND || op == TK_OR;
}

TACVar *gen_global(TACMember *value)
{
  stringstream name_ss;
  name_ss << "gbl_" << last_global_index++;
  
  TACVar *global = new TACVar(name_ss.str());
  
  prog.globals.push_back(global);
  
  if (value != NULL)
  {
    TACAttr *attr = new TACAttr(global, value);
    prog.global_attrs.push_back(attr);
  }
  
  return global;
}

TACVar *gen_temp()
{
  return new TACVar("t", last_temp_index++);
}

BasicBlock *gen_basic_block(CFG *cfg)
{
  BasicBlock *basic_block = new BasicBlock();

  cfg->blocks.push_back(basic_block);
  
  return basic_block;
}

TACMember *gen_attr_binop(TACVar *target, Exp *ast_expression, CFG *cfg)
{
  TACMember *left = gen_operations(NULL, ast_expression->u.binop.e1, cfg);
  TACMember *right = gen_operations(NULL, ast_expression->u.binop.e2, cfg);
  
  int op = ast_expression->u.binop.op;
  
  if (target == NULL)
  {
    target = gen_temp();
  }
  
  TACOperation *operation = new TACAttr(target, left, op, right);
  
  cfg->work_block->ops.push_back(operation);
  
  return target;
}

TACMember *gen_short_circuit(TACVar *target, Exp *ast_expression, CFG *cfg)
{
  if (ast_expression->u.binop.op == TK_AND)
  {
    BasicBlock *new_block = gen_basic_block(cfg);
    
    cfg->work_block->br(new_block);
    
    cfg->work_block = new_block;
    
    if (target == NULL)
    {
      target = gen_temp();
    }
    
    gen_operations(target, ast_expression->u.binop.e1, cfg);
    
    new_block = gen_basic_block(cfg);
    BasicBlock *final = gen_basic_block(cfg);
    
    cfg->work_block->brc(target, new_block, final);
    cfg->work_block = new_block;
    
    gen_operations(target, ast_expression->u.binop.e2, cfg);
    
    cfg->work_block->br(final);
    cfg->work_block = final;
    
    return target;
  }
  else
  {
    BasicBlock *new_block = gen_basic_block(cfg);
    
    cfg->work_block->br(new_block);
    
    cfg->work_block = new_block;
    
    if (target == NULL)
    {
      target = gen_temp();
    }
    
    gen_operations(target, ast_expression->u.binop.e1, cfg);
    
    new_block = gen_basic_block(cfg);
    BasicBlock *final = gen_basic_block(cfg);
    
    cfg->work_block->brc(target, final, new_block);
    cfg->work_block = new_block;
    
    gen_operations(target, ast_expression->u.binop.e2, cfg);
    
    cfg->work_block->br(final);
    cfg->work_block = final;
    
    return target;
  }
}

TACMember *gen_operations(TACVar *target, Exp *ast_expression, CFG *cfg)
{
  if (ast_expression == NULL) {
    return NULL;
  }
  
  switch(ast_expression->tag)
  {
    case EXP_INT: {
      TACMember *literal = new TACLiteral<int>(ast_expression->u.ival);
      
      if (target != NULL)
      {
        TACOperation *operation = new TACAttr(target, literal);
        cfg->work_block->ops.push_back(operation);
      }
      
      return literal;
      break;
    }
    case EXP_VAR: {
      TACMember *var = gen_var(ast_expression->u.var);
      
      if (target != NULL)
      {
        TACOperation *operation = new TACAttr(target, var);
        cfg->work_block->ops.push_back(operation);
      }
      
      return var;
      break;
    }
    case EXP_BINOP: {
      if (can_short_circuit(ast_expression->u.binop.op))
      {
        return gen_short_circuit(target, ast_expression, cfg);
      }
      else
      {
        return gen_attr_binop(target, ast_expression, cfg);
      }
      
      break;
    }
    case EXP_FUNCALL: {
      string name = ast_expression->u.funcall.name;
      ExpListNode *ast_params = ast_expression->u.funcall.expl;

      vector<TACMember *> params;

      while (ast_params != NULL)
      {
        Exp *param_exp = ast_params->exp;
        
        TACMember *param = gen_operations(NULL, param_exp, cfg);
        
        params.push_back(param);
        
        ast_params = ast_params->next;
      }
      
      TACMember *funcall = new TACFuncall(name, params);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACOperation *attr = new TACAttr(target, funcall);
      
      cfg->work_block->ops.push_back(attr);
      
      return target;
      break;
    }
    case EXP_NEG : {
      TACMember *neg = gen_operations(NULL, ast_expression->u.exp, cfg);
      
      TACMember *zero = new TACLiteral<int>(0);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACOperation *attr = new TACAttr(target, zero, '-', neg);
      
      cfg->work_block->ops.push_back(attr);
      
      return target;
      break;
    }
    case EXP_STRING: {
      TACMember *literal = new TACLiteral<string>(ast_expression->u.sval);
      
      TACVar *global = gen_global(literal);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACOperation *load = new Load(target, global);
      
      cfg->work_block->ops.push_back(load);
      
      return target;
      break;
    }
    case EXP_CONV: {
      error("CONV Expression Handling not supported!");
      break;
    }
    default: {
      error("Unhandled expression type", ast_expression);
      break;
    }
  }
}

TACOperation *gen_return_operation(Exp *ast_expression, CFG *cfg)
{
  TACMember *ret_value = gen_operations(NULL, ast_expression, cfg);
  TACOperation *ret = new TACReturn(ret_value);
  
  return ret;
}

BasicBlock *gen_commands(Block *ast_block, CFG *cfg)
{
  CommListNode *ast_commands;
  
  ast_commands = ast_block->comms;
  
  BasicBlock *block = gen_basic_block(cfg);
  
  cfg->work_block = block;
  
  while (ast_commands != NULL)
  {
    Command *ast_command;
    ast_command = ast_commands->comm;
    
    switch(ast_command->tag)
    {
      case COMMAND_ATTR: {
        TACVar *target = gen_var(ast_command->u.attr.lvalue);

        gen_operations(target, ast_command->u.attr.rvalue, cfg);

        break;
      }
      case COMMAND_RET: {
        TACOperation *ret;
        
        ret = gen_return_operation(ast_command->u.ret, cfg);
        
        cfg->work_block->ops.push_back(ret);
        
        break;
      }
      default: {
        error("Unhandled command type.", ast_command);
        break;
      }
    }
    
    ast_commands = ast_commands->next;
  }
  
  return cfg->work_block;
}

CFG *gen_cfg(Declr *ast_declr)
{
  CFG *cfg = new CFG(ast_declr->u.name);
  
  gen_commands(ast_declr->u.func.block, cfg);
  
  return cfg;
}

Prog gen_prog(DeclrListNode *ast_declrs)
{
  while(ast_declrs != NULL)
  {
    if (ast_declrs->declr->tag == DECLR_FUNC)
    {
      CFG *cfg = gen_cfg(ast_declrs->declr);
      prog.cfgs.push_back(cfg);
    }
    
    ast_declrs = ast_declrs->next;
    last_index = 0;
  }
  
  return prog;
}

