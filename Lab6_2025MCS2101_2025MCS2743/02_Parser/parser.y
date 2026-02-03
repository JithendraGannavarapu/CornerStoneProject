%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"  

#define MAX_VARS 1000
int parsing_failed = 0;
extern int yylex();
extern int line_num; 
void yyerror(const char *s);

ASTNode* root;
  
char* symbol_table[MAX_VARS]; 
int count = 0;

void clear_parser_symbols() {
    for (int i = 0; i < count; i++) {
        if (symbol_table[i]) {
            free(symbol_table[i]);
            symbol_table[i] = NULL;
        }
    }
    count = 0;
    parsing_failed = 0;
    root = NULL;
}

void add_symbol(char* name) {
    if (count >= MAX_VARS) {
        parsing_failed = 1;
        return;
    }
    for(int i=0; i<count; i++) {
        if(strcmp(symbol_table[i], name) == 0) {
            fprintf(stderr, "Error: Variable '%s' already declared at line %d\n", name, line_num);
            parsing_failed = 1;
            return;
        }
    }
    symbol_table[count] = strdup(name);
    count++;
}

void check_symbol(char* name) {
    int found = 0;
    for(int i=0; i<count; i++) {
        if(strcmp(symbol_table[i], name) == 0) {
            found = 1;
            break;
        }
    }
    if(!found) {
        fprintf(stderr, "Error: Undeclared variable '%s' used at line %d\n", name, line_num);
        parsing_failed = 1; 
    }
}
%}

%union {
    int ival;           
    char* sval;         
    struct ASTNode* nval; 
}

%token <sval> IDENTIFIER
%token <ival> INTEGER
%token VAR IF ELSE WHILE PRINT
%token ASSIGN
%token PLUS MINUS MULT DIV
%token EQ NEQ LT GT LE GE
%token SEMI LPAREN RPAREN LBRACE RBRACE

%type <nval> program statement_list statement block
%type <nval> expression equality comparison term factor unary primary

%start program

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%left EQ NEQ LT GT LE GE
%left PLUS MINUS
%left MULT DIV
%right NEG

%%

program:
    statement_list { root = $1; }
    ;

statement_list:
    statement_list statement { 
        $$ = create_node_list($1, $2); 
    }
    | { $$ = NULL; }
    ;

statement:
    VAR IDENTIFIER SEMI { 
        add_symbol($2);           
        $$ = create_var_decl($2, NULL); 
    }
    | VAR IDENTIFIER ASSIGN expression SEMI { 
        add_symbol($2);          
        $$ = create_var_decl($2, $4); 
    }
    | IDENTIFIER ASSIGN expression SEMI { 
        check_symbol($1);         
        $$ = create_assign($1, $3); 
    }
    | PRINT expression SEMI {
        $$ = create_unary("print", $2);
    }
    | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE { 
        $$ = create_if($3, $5, NULL); 
    }
    | IF LPAREN expression RPAREN statement ELSE statement { 
        $$ = create_if($3, $5, $7); 
    }
    | WHILE LPAREN expression RPAREN statement { 
        $$ = create_while($3, $5); 
    }
    | block { $$ = $1; }
    ;

block:
    LBRACE statement_list RBRACE { $$ = $2; }
    ;

expression:
    equality { $$ = $1; }
    ;

equality:
    comparison { $$ = $1; }
    | equality EQ comparison { $$ = create_binop("==", $1, $3); }
    | equality NEQ comparison { $$ = create_binop("!=", $1, $3); }
    ;

comparison:
    term { $$ = $1; }
    | comparison LT term { $$ = create_binop("<", $1, $3); }
    | comparison GT term { $$ = create_binop(">", $1, $3); }
    | comparison LE term { $$ = create_binop("<=", $1, $3); }
    | comparison GE term { $$ = create_binop(">=", $1, $3); }
    ;

term:
    factor { $$ = $1; }
    | term PLUS factor { $$ = create_binop("+", $1, $3); }
    | term MINUS factor { $$ = create_binop("-", $1, $3); }
    ;

factor:
    unary { $$ = $1; }
    | factor MULT unary { $$ = create_binop("*", $1, $3); }
    | factor DIV unary  { $$ = create_binop("/", $1, $3); }
    ;

unary:
    PLUS unary %prec NEG { $$ = create_unary("+", $2); }
    | MINUS unary %prec NEG { $$ = create_unary("-", $2); }
    | primary { $$ = $1; }
    ;

primary:
    INTEGER { $$ = create_int_node($1); }
    | IDENTIFIER { 
        check_symbol($1);          
        $$ = create_id_node($1); 
    }
    | LPAREN expression RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", line_num, s);
}