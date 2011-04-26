#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decl.h"
#include "ast.h"

extern int yylineno;
extern FILE *outfile;
extern char *filename;

static int token;

static CommListNode *commandl();
static Exp *simple();
static Block *block();

static int syntax_error(char *message) {
        printf("Syntax error in line %i: %s\n", yylineno, message);
        exit(0);
}

static void match(int next) {
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

#define NO_BINOP -1

static int binop(int token) {
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

static int unop(int token) {
	switch(token) {
		case '-': return 4;
		case '!': return 2;
	}
}

static Exp *expr(int level) {
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

static ExpListNode *exprl(int starter, int separator, int finisher) {
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

static Var *var(char *name) {
        Var *var;
        
        ALLOC(var, Var);
        var->name = name;
        
        if(token == '[') {
                ExpListNode *idxs;
                
                token = yylex();
                idxs = exprl('[',']', ']');
                
                if (idxs == NULL) syntax_error("an array must have an index.");
                
                var->name = name;
	        var->idxs = idxs;
	
	}
        
        return var;
}

static Exp *funcall(char *name, Exp *exp) {
        if (exp == NULL) ALLOC(exp, Exp);
	
	exp->tag = EXP_FUNCALL;
        
        exp->u.funcall.name = name;
	exp->u.funcall.expl = exprl(0, ',', ')');
	exp->u.funcall.func = NULL;
	
	return exp;
}

static Exp *simple() {
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
			printf("invalid expression, token: %c\n", token);
			exit(0);
		}
	}

	return exp;
}

static int isTypeToken() {
        return token == TK_TINT ||
               token == TK_TFLOAT ||
               token == TK_TCHAR ||
               token == TK_TVOID;
}

static IntListNode *sizes(Type *type) {
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
                        syntax_error("invalid array size.");
                        break;
                }
        }
        
        match(']');
        
        this->next = NULL;
        
        type->dimensions = 1; /*TODO: arrays com várias dimensioes!*/
        
        return this;
}

static Type *type() {
        Type *this;
        IntListNode *first;
        
        ALLOC(this, Type);
        this->type = token;
        this->line = yylineno;
        
        token = yylex();
        
        this->sizes = sizes(this);
        
        return this;
}

static Declr *declr_func(char *name) {
        Declr *this;

        ALLOC(this, Declr);
        
        this->tag = DECLR_FUNC;
        ALLOCS(this->u.name, strlen(name) + 1);
        strcpy(this->u.name, name);
        match(')'); /*TODO: Funções com parâmetros*/
        
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
                        syntax_error("invalid function declaration.");
                        break;
                }
        }
        
        return this;

}

static Declr *declr_var(char *name) {
        Declr *this;

        ALLOC(this, Declr);
        
        this->tag = DECLR_VAR;
        ALLOCS(this->u.name, strlen(name) + 1);
        strcpy(this->u.name, name);
        match(';');
        
        return this;
}

static Declr *declr(DeclrListNode *declrs, int from_block) {
        Declr *this;
        Type *declr_type;
        char *name;
        
       declr_type = type();
                
                
       ALLOCS(name, strlen(yyval.sval) + 1);
       strcpy(name, yyval.sval);
       
       token = yylex();
       
        switch (token) {
                case '(': {
                        if (from_block) {
                                syntax_error("cannot declare functions inside blocks!");
                        }
                        token = yylex();
                        this = declr_func(name);
                        this->type = declr_type;
                        break;
                }
                case ',': {
                        /*TODO: Múltiplas declarações de variáveis*/
                        break;
                }
                case ';': {
                        this = declr_var(name);
                        this->type = declr_type;
                        break;
                }
                default: {
                        syntax_error("invalid declaration.");
                        break;
                }
        }        
}

static DeclrListNode *declrs(int from_block) {
        DeclrListNode *first;
        DeclrListNode *curr, *prev, *next;

        if (!isTypeToken()) {
                return NULL;
        }

        ALLOC(first, DeclrListNode);
        first->declr = declr(first, from_block);

        prev = first;

        ALLOC(curr, DeclrListNode);
        ALLOC(next, DeclrListNode);
        
        while (isTypeToken()) {
                curr->declr = declr(curr, from_block);
                prev->next = curr;
                prev = curr;
                curr = next;
        }

        return first;
}

static Block* block() {
        Block *this;
        ALLOC(this, Block);
        
        this->declrs = declrs(1);
        
        this->comms = commandl();
        
        match('}');
        
        return this;
}

static Command *command() {
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
			 printf("invalid command, funcall or attr\n");
			 exit(0);
		 }
		 break;
	 }

     default: {
		 printf("invalid command in line %i, (token: %c)\n", yylineno, token);
		 exit(0);
	 }
  }

  return this;
}

static CommListNode *commandl() {
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

int main(int argc, char **argv) {
  FILE *f;
  //CommListNode* commands;
  DeclrListNode *declr_list;
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
  yyrestart(f);
  yylineno = 1;
  outfile = stdout;
  filename = "stdout";
  token = yylex();
 /* commands = commandl();
  print_commlist(0, commands);*/
  
  declr_list = declrs(0);
  print_declrlist(0, declr_list);
  
  
}
