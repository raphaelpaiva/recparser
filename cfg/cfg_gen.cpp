#include <cstdlib>
#include <iostream>
#include "cfg_gen.h"
#include "ast_utils.h"

using namespace std;

TACMember *gen_operations(TACVar *target, Exp *ast_expression, CFG *cfg);
TACMember *gen_attr_binop(TACVar *target, Exp *ast_expression, CFG *cfg);
void parse_ast_commands(CommListNode *ast_commands, CFG *cfg);

static int last_temp_index;
static int last_global_index;

Prog prog;

TACVar *gen_var(Var *ast_var)
{
  stringstream ss;
  
  ss << ast_var->name << "_" << ast_var->var;
  
  return new TACVar(ss.str());
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
  
  TACVar *new_target = new TACVar(target);
  
  Operation *operation = new TACAttr(new_target, left, op, right);
  
  cfg->work_block->ops.push_back(operation);
  cfg->work_block->vars.insert(new_target);
  
  return new_target;
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
    
    TACVar *new_target = new TACVar(target);
    
    gen_operations(new_target, ast_expression->u.binop.e1, cfg);
    
    new_block = gen_basic_block(cfg);
    BasicBlock *final = gen_basic_block(cfg);
    
    cfg->work_block->brc(new_target, new_block, final);
    cfg->work_block = new_block;
    
    gen_operations(new_target, ast_expression->u.binop.e2, cfg);
    
    cfg->work_block->br(final);
    cfg->work_block = final;
    
    return new_target;
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
    
    TACVar *new_target = new TACVar(target);
    
    gen_operations(new_target, ast_expression->u.binop.e1, cfg);
    
    new_block = gen_basic_block(cfg);
    BasicBlock *final = gen_basic_block(cfg);
    
    cfg->work_block->brc(new_target, final, new_block);
    cfg->work_block = new_block;
    
    gen_operations(new_target, ast_expression->u.binop.e2, cfg);
    
    cfg->work_block->br(final);
    cfg->work_block = final;
    
    return new_target;
  }
}

vector<TACMember *> gen_funcall_params(ExpListNode *ast_params, CFG *cfg)
{
  vector<TACMember *> params;

  while (ast_params != NULL)
  {
    Exp *param_exp = ast_params->exp;
    
    TACMember *param = gen_operations(NULL, param_exp, cfg);
    
    params.push_back(param);
    
    ast_params = ast_params->next;
  }
  
  return params;
}

TACMember *gen_operations(TACVar *target, Exp *ast_expression, CFG *cfg)
{
  if (ast_expression == NULL) {
    return NULL;
  }
  
  switch(ast_expression->tag)
  {
    case EXP_INT: {
      TACMember *literal = new Literal<int>(ast_expression->u.ival);
      
      if (target != NULL)
      {
        TACVar *new_target = new TACVar(target);
        Operation *operation = new TACAttr(new_target, literal);
        cfg->work_block->ops.push_back(operation);
        cfg->work_block->vars.insert(new_target);
      }
      
      return literal;
      break;
    }
    case EXP_VAR: {
      TACMember *var = gen_var(ast_expression->u.var);
      
      if (target != NULL)
      {
        TACVar *new_target = new TACVar(target);
        Operation *operation = new TACAttr(new_target, var);
        cfg->work_block->ops.push_back(operation);
        cfg->work_block->vars.insert(new_target);
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

      vector<TACMember *> params = gen_funcall_params(ast_params, cfg);
      
      TACMember *funcall = new TACFuncall(name, params);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACVar *new_target = new TACVar(target);
      
      Operation *attr = new TACAttr(new_target, funcall);
      
      cfg->work_block->ops.push_back(attr);
      cfg->work_block->vars.insert(new_target);
      
      return new_target;
      break;
    }
    case EXP_NEG : {
      TACMember *neg = gen_operations(NULL, ast_expression->u.exp, cfg);
      
      TACMember *zero = new Literal<int>(0);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACVar *new_target = new TACVar(target);
      
      Operation *attr = new TACAttr(new_target, zero, '-', neg);
      
      cfg->work_block->ops.push_back(attr);
      cfg->work_block->vars.insert(new_target);
      
      return new_target;
      break;
    }
    case EXP_STRING: {
      TACMember *literal = new Literal<string>(ast_expression->u.sval);
      
      TACVar *global = gen_global(literal);
      
      if (target == NULL)
      {
        target = gen_temp();
      }
      
      TACVar *new_target = new TACVar(target);
      
      Operation *attr = new TACAttr(new_target, global);
      
      cfg->work_block->ops.push_back(attr);
      cfg->work_block->vars.insert(new_target);
      
      return new_target;
      break;
    }
    default: {
      error("Unhandled expression type", ast_expression);
      break;
    }
  }
}

Operation *gen_return_operation(Exp *ast_expression, CFG *cfg)
{
  TACMember *ret_value = gen_operations(NULL, ast_expression, cfg);
  Operation *ret = new Return(ret_value);
  
  return ret;
}

void parse_ast_command(Command *ast_command, CFG *cfg)
{
  switch(ast_command->tag)
  {
    case COMMAND_ATTR: {
      TACVar *target = gen_var(ast_command->u.attr.lvalue);

      gen_operations(target, ast_command->u.attr.rvalue, cfg);

      break;
    }
    case COMMAND_RET: {
      Operation *ret;
      
      ret = gen_return_operation(ast_command->u.ret, cfg);
      
      cfg->work_block->ret(ret);
      
      break;
    }
    case COMMAND_FUNCALL: {
      TACFuncall *tac_funcall = new TACFuncall(ast_command->u.funcall->u.funcall.name,
                                               gen_funcall_params(ast_command->u.funcall->u.funcall.expl, cfg));

      Operation *funcall = new Funcall(tac_funcall);
      
      cfg->work_block->ops.push_back(funcall);
     
      break;
    }
    case COMMAND_WHILE: {
      BasicBlock *test = gen_basic_block(cfg);
      cfg->work_block->br(test);
      cfg->work_block = test;
      
      TACMember *cond = gen_operations(NULL, ast_command->u.cwhile.exp, cfg);
      
      BasicBlock *body = gen_basic_block(cfg);
      BasicBlock *final = gen_basic_block(cfg);
      
      cfg->work_block->brc(cond, body, final);
      cfg->work_block = body;
      
      parse_ast_command(ast_command->u.cwhile.comm, cfg);
      
      cfg->work_block->br(test);
      cfg->work_block = final;
      
      break;
    }
    case COMMAND_IF: {
      BasicBlock *body_block;
      BasicBlock *next_block;
      
      BasicBlock *test = gen_basic_block(cfg);
      
      cfg->work_block->br(test);
      cfg->work_block = test;
      
      TACMember *cond = gen_operations(NULL, ast_command->u.cif.exp, cfg);

      body_block = gen_basic_block(cfg);
      
      next_block = gen_basic_block(cfg);
      
      cfg->work_block->brc(cond, body_block, next_block);
      cfg->work_block = body_block;
      
      parse_ast_command(ast_command->u.cif.comm, cfg);
      
      if (ast_command->u.cif.celse == NULL)
      {
        cfg->work_block->br(next_block);
        cfg->work_block = next_block;
      }
      else
      {
        BasicBlock *final = gen_basic_block(cfg);
        cfg->work_block->br(final);
        
        cfg->work_block = next_block;
        parse_ast_command(ast_command->u.cif.celse, cfg);
        
        cfg->work_block->br(final);
        cfg->work_block = final;
      }
      
      break;
    }
    case COMMAND_BLOCK: {
      parse_ast_commands(ast_command->u.block->comms, cfg);
      break;
    }
    default: {
      error("Unhandled command type.", ast_command);
      break;
    }
  }
    
}

void parse_ast_commands(CommListNode *ast_commands, CFG *cfg)
{
  while (ast_commands != NULL)
  {
    Command *ast_command = ast_commands->comm;
    
    parse_ast_command(ast_command, cfg);
    
    ast_commands = ast_commands->next;
  }
}

string gen_cfg_name(string name, DeclrListNode *ast_params)
{
  if(ast_params == NULL)
  {
    return name + "()";
  }
  
  stringstream ss;
  
  ss << name << "(";
  
  DeclrListNode *curr = ast_params;
  
  while(curr != NULL)
  {
    Declr *declr = curr->declr;
    
    ss << "i32 %" << declr->u.name << "_" << declr << ".0" << ", ";
    
    curr = curr->next;
  }
  
  long pos = ss.tellp();
    
  ss.seekp(pos - 2);
  
  ss << ")";
  
  return ss.str();
}

CFG *gen_cfg(Declr *ast_declr)
{
  CFG *cfg = new CFG(gen_cfg_name(ast_declr->u.name, ast_declr->u.func.params));
  
  BasicBlock *block = gen_basic_block(cfg);
  cfg->work_block = block;
  
  Block *ast_block = ast_declr->u.func.block;
  
  parse_ast_commands(ast_block->comms, cfg);
  
  return cfg;
}

Prog gen_prog(DeclrListNode *ast_declrs)
{
  while(ast_declrs != NULL)
  {
    if (ast_declrs->declr->tag == DECLR_FUNC && ast_declrs->declr->u.func.block != NULL)
    {
      CFG *cfg = gen_cfg(ast_declrs->declr);
      prog.cfgs.push_back(cfg);
    }
    
    ast_declrs = ast_declrs->next;
    last_index = 0;
  }
  
  return prog;
}

