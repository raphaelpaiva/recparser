#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"

void match(int next) {
	if(token != next) {
		printf("missing token: %c was %c\n", next, token);
		exit(0);
	}

	token = yylex();
}

struct { int left; int right; }
pri[] = {
	{4, 4}, /* + - */
	{5, 5}, /* * / */
	{3, 3}, /* == != < > <= >= ! */
	{6, 5}, /* ^ */
	{2, 2}, /* && */
	{1, 1}, /* || */
};

int binop(int token) {
	switch(token) {
		case '+': 	return 0;
		case '-': 	return 0;
		case '*': 	return 1;
		case '/': 	return 1;
		case '^':       return 3;
		case TK_EQ:     return 2;
		case '<':       return 2;
		case '>':       return 2;
		case TK_LEQ:    return 2;
		case TK_GEQ:    return 2;
		case TK_NEQ:    return 2;
		case TK_AND:    return 4;
		case TK_OR:     return 5;
		default:    return NO_BINOP;
	}
}

int unop(int token) {
	switch(token) {
		case '-': return 4;
		case '!': return 2;
	}
}

Exp *expr(int level) {
	Exp *exp1 = simple();
	
	int op = binop(token);

	while(op != NO_BINOP && pri[op].left > level) {
		Exp *new, *exp2;
		int top = token;
		token = yylex();
		exp2 = expr(pri[op].right);

		op = binop(token);

		ALLOC(new, Exp);
		new->tag = EXP_BINOP;
		new->u.binop.op = top;
		new->u.binop.e1 = exp1;
		new->u.binop.e2 = exp2;

		exp1 = new;
	}

	return exp1;
}

ExpListNode *exprl(int starter, int separator, int finisher) {
        ExpListNode *root;
        
        if (token == finisher) return NULL;
        
        ALLOC(root, ExpListNode);
        root->exp = expr(0);
        root->next = NULL;
        
        while(token == separator) {
                token = yylex();
                
                if (starter && token != starter ) break;
                if (starter) match(starter);
                
                root->next = exprl(separator, starter, finisher);
        }
        
        return root;        
}

Var *var(char *name) {
        Var *var;
        
        ALLOC(var, Var);
        var->name = name;
        
        if(token == '[') {
                ExpListNode *idxs;
                
                token = yylex();
                idxs = exprl('[',']', ']');
                
                if (idxs == NULL) SYNTAX_ERROR("an array must have an index.");
                
                var->name = name;
	        var->idxs = idxs;
	
	}
        
        return var;
}

Exp *funcall(char *name, Exp *exp) {
        if (exp == NULL) ALLOC(exp, Exp);
	
	exp->tag = EXP_FUNCALL;
        
        exp->u.funcall.name = name;
	exp->u.funcall.expl = exprl(0, ',', ')');
	exp->u.funcall.func = NULL;
	
	return exp;
}

Exp *simple() {
	Exp *exp;

	switch(token) {
		case TK_ID: {
			char *name;
			
			ALLOCS(name, strlen(yyval.sval) + 1);
			strcpy(name, yyval.sval);
			token = yylex();
			
			if (token == '(') { /* Chamada de função*/
			        token = yylex();
				exp = funcall(name, NULL);
				match(')');
			}
		        else { /* Var ou array */
		                ALLOC(exp, Exp);
	                        exp->tag = EXP_VAR;
	                        exp->u.var = var(name);
		        }
			
			break;
		}

		case TK_INT: {
			ALLOC(exp, Exp);
			exp->tag = EXP_INT;
			exp->u.ival = yyval.ival;
			token = yylex();
			break;
		}
		
		case '(': {
			token = yylex();
			exp = expr(0);
			match(')');
			break;
		}

		case '-': {
			token = yylex();
			ALLOC(exp, Exp);
			exp->tag = EXP_NEG;
			exp->u.exp = expr(unop('-'));
			break;
		}
		case '!': {
			token = yylex();
			ALLOC(exp, Exp);
			exp->tag = EXP_LNEG;
			exp->u.exp = expr(unop('!'));
		}

		default: {
		        SYNTAX_ERROR("invalid expression, token: %c", token);
		}
	}

	return exp;
}

int is_type_token() {
        return token == TK_TINT ||
               token == TK_TFLOAT ||
               token == TK_TCHAR ||
               token == TK_TVOID;
}

IntListNode *sizes(Type *type) {
        IntListNode *this;

        if (token != '[') return NULL;
        token = yylex();
        
        switch (token) {
                case ']': {
                        ALLOC(this, IntListNode);
                        this->n = 0;
                        break;
                }
                case TK_INT: {
                        token = yylex();
                        ALLOC(this, IntListNode);
                        this->n = yyval.ival;
                        break;
                }
                default: {
                        SYNTAX_ERROR("invalid array size.");
                        break;
                }
        }
        
        match(']');
        
        this->next = NULL;
        
        type->dimensions++;
        
        return this;
}

Type *type() {
        Type *this;
        IntListNode *first, *curr;
        
        ALLOC(this, Type);
        this->type = token;
        this->line = yylineno;
        
        token = yylex();
        
        this->dimensions = 0;
        
        first = sizes(this);
        
        ALLOC(curr, IntListNode);
        curr = first;
        
        while (token == '[') {
                IntListNode *next;
                ALLOC(next, IntListNode);
                
                next = sizes(this);
                
                curr->next = next;
                curr = next;
        }
        
        this->sizes = first;
        
        return this;
}

Declr *declr_func(char *name, Type *type) {
        Declr *this;

        ALLOC(this, Declr);
        
        this->tag = DECLR_FUNC;
        ALLOCS(this->u.name, strlen(name) + 1);
        strcpy(this->u.name, name);

        if (is_type_token() || token == TK_MANY) {
                ALLOC(this->u.func.params, DeclrListNode);
                declr(this->u.func.params, 0, 1);
                
        } else {
                match(')');
        }
        
        switch(token) {
                case ';': {
                        token = yylex();
                        this->u.func.block = NULL;
                        break;
                }
                case '{': {
                        token = yylex();
                        this->u.func.block = block();
                        break;
                }
                default: {
                        SYNTAX_ERROR("invalid function declaration: %s", name);
                        break;
                }
        }
        
        this->type = type;
        
        return this;

}

Declr *declr_var(char *name, Type *type) {
        Declr *this;

        if (type->type == TK_TVOID) {
                SYNTAX_ERROR("Cannot declare a variable of void type: %s", name);
        }
        
        if (type->type == TK_MANY) {
                return NULL;
        }
        
        ALLOC(this, Declr);
        
        this->tag = DECLR_VAR;
        ALLOCS(this->u.name, strlen(name) + 1);
        strcpy(this->u.name, name);
        this->type = type;
        
        return this;
}

char *get_TK_ID_name() {
        char *name;
        
        match(TK_ID);
                
        ALLOCS(name, strlen(yyval.sval) + 1);
        strcpy(name, yyval.sval);
        
        return name;
}

Declr *declr(DeclrListNode *root, int from_block, int from_function) {
       Type *declr_type;
       char *name;
       
       declr_type = type();
       
       if (declr_type->type == TK_MANY) {
                token = yylex();
       } else {
                name = get_TK_ID_name();
       }
       
       switch (token) {
                case '(': {
                        if (from_block) {
                                SYNTAX_ERROR("cannot declare functions inside blocks: %s", name);
                        }
                        
                        if (from_function) {
                                SYNTAX_ERROR("cannot declare functions as function parameters: %s", name);
                        }
                        
                        token = yylex();
                        root->declr = declr_func(name, declr_type);
                        break;
                }
                case ',': {
                        DeclrListNode *curr;
                        
                        root->declr = declr_var(name, declr_type);
                        
                        ALLOC(curr, DeclrListNode);
                        curr = root;
                        while(token == ',') {
                                DeclrListNode *next;
                                char *name;
                                
                                if (declr_type->type == TK_MANY) {
                                        SYNTAX_ERROR("varargs should be the last parameter of a function");
                                }
                                
                                token = yylex();
                                
                                if (from_function) {
                                        declr_type = type();
                                }
                                
                                if (declr_type->type != TK_MANY) {
                                        name = get_TK_ID_name();
                                }
                                
                                ALLOC(next, DeclrListNode);
                                next->declr = declr_var(name, declr_type);
                                next->next = NULL;
                                
                                curr->next = next;
                                
                                curr = next;
                        }
                        
                        if (from_function) {
                                match(')');
                                break;
                        }
                        
                        match(';');
                        break;
                }
                
                case ')': {
                        if (!from_function) {
                                SYNTAX_ERROR("invalid declaration: %s", name);
                        }
                        
                        root->declr = declr_var(name, declr_type);
                        match(')');
                        break;
                }
                
                case ';': {
                        root->declr = declr_var(name, declr_type);
                        
                        if (declr_type->type != TK_MANY) {
                                match(';');
                        }
                        break;
                }
                default: {
                        SYNTAX_ERROR("invalid declaration: %s", name);
                        break;
                }
       }
        
       return root->declr;
}

DeclrListNode *declrs(int from_block, int from_function) {
        DeclrListNode *first, *curr;

        if (!is_type_token()) {
                return NULL;
        }

        ALLOC(first, DeclrListNode);
        first->declr = declr(first, from_block, from_function);

        ALLOC(curr, DeclrListNode);
        
        curr = first;
        
        while(curr->next) curr = curr->next;

        while (is_type_token()) {
                DeclrListNode *next;
                
                ALLOC(next, DeclrListNode);
                
                next->declr = declr(next, from_block, from_function);
                
                curr->next = next;
                curr = next;
                while(curr->next) curr = curr->next;
        }
        
        return first;
}

Block* block() {
        Block *this;
        ALLOC(this, Block);
        
        this->declrs = declrs(1, 0);
        
        this->comms = commandl();
        
        match('}');
        
        return this;
}

Command *command() {
  Command *this;
  ALLOC(this, Command);

  switch (token) {
	 case TK_IF: {
		 token = yylex();
		 this->tag = COMMAND_IF;
		 match('(');
		 this->u.cif.exp = expr(0);
		 match(')');
		 this->u.cif.comm = command();
		  
		 if(token == TK_ELSE) {
			token = yylex();
		 	this->u.cif.celse = command();
		 }
		  
		 break;
	 }

	 case TK_WHILE: {
		 token = yylex();
		 this->tag = COMMAND_WHILE;
		 match('(');
		 this->u.cwhile.exp = expr(0);
		 match(')');
		 this->u.cwhile.comm = command();
		 break;
	 }

	 case TK_RETURN: {
		 token = yylex();
		 this->tag = COMMAND_RET;
		 
		 if (token != ';') {
			 this->u.ret = expr(0);
		 }

		 match(';');

		 break;
	 }
	 
	 case ';': {
	 	token = yylex();
		this->tag = COMMAND_BLOCK;
		break;
	 }

	 case '{': {
		 token = yylex();
		 this->tag = COMMAND_BLOCK;
		 
		 if (token != '}') {
		         this->u.block = block();
		 }

		 break;
	 }

	 case TK_ID: {
		 char *name;
		 Var *cvar;
		 ALLOCS(name, strlen(yyval.sval) + 1);
		 strcpy(name, yyval.sval);
		 
		 token = yylex();
		 
		 cvar = var(name);

		 if (token == '(') { /* Chamada de Função */
	        	 token = yylex();

			 this->tag = COMMAND_FUNCALL;
			 this->u.funcall = funcall(name, this->u.funcall);
			 
			 match(')'); match(';');
		 } else if(token == '=') { /* Atribuição */
			 token = yylex();

			 this->tag = COMMAND_ATTR;
			 this->u.attr.lvalue = cvar;
			 this->u.attr.rvalue = expr(0);

			 match(';');
		 } else {
			 SYNTAX_ERROR("invalid command, funcall or attr: %s", name);
		 }
		 break;
	 }

     default: {
		 SYNTAX_ERROR("invalid command, token: %c", token);
	 }
  }

  return this;
}

CommListNode *commandl() {
  CommListNode *first, *curr;
  
  if (token == '}') return NULL;
  
  ALLOC(first, CommListNode);
  first->comm = command();
  first->next = NULL;
  
  curr = first;
  
  while(token && token != '}') {
    CommListNode *next;
    ALLOC(next, CommListNode);
    next->comm = command();
    next->next = NULL;
    curr->next = next;
    curr = next;
  }
  return first;
}

FILE *openFile(int argc, char **argv)
{
  FILE *f;
  
  if(argc > 1) {
    f = fopen(argv[1], "r");
    filename = argv[1];
  } else {
    f = stdin;
    filename = "stdin";
  }
 
  if(!f) {
    fprintf(stderr, "Cannot open file %s. Exiting...", filename);
    exit(0);
  }
  
  return f;
}

void startLex(FILE *f)
{
  yyrestart(f);
  yylineno = 1;
  outfile = stdout;
  filename = "stdout";
  token = yylex();
}
/*
int main(int argc, char **argv) {
  FILE *f;
  DeclrListNode *declr_list;
  
  f = openFile(argc, argv);
  
  startLex(f);
  
  declr_list = declrs(0, 0);
  print_declrlist(0, declr_list);
  
  return 0;
}*/
