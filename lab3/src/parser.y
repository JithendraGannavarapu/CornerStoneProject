%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"  /* Include our AST header */

/* External functions from Lexer */
extern int yylex();
extern int line_num; /* Tracked in lexer.l */
void yyerror(const char *s);

/* Root of the AST */
ASTNode* root;

/* -------------------------------------------------------
   Symbol Table Logic
   ------------------------------------------------------- */
char* symbol_table[100]; /* Array to store variable names */
int count = 0;

/* Function to add a variable to the table */
void add_symbol(char* name) {
    /* Check if already declared to avoid duplicates (optional) */
    for(int i=0; i<count; i++) {
        if(strcmp(symbol_table[i], name) == 0) {
            fprintf(stderr, "Error: Variable '%s' already declared at line %d\n", name, line_num);
            exit(1);
        }
    }
    symbol_table[count] = strdup(name);
    count++;
}

/* Function to check if a variable exists */
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
        exit(1); /* Stop parsing immediately */
    }
}
%}

/* Define the Union */
%union {
    int ival;           /* For INTEGER tokens */
    char* sval;         /* For IDENTIFIER tokens */
    struct ASTNode* nval; /* For all grammar rules */
}

/* Token Definitions */
%token <sval> IDENTIFIER
%token <ival> INTEGER
%token VAR IF ELSE WHILE
%token ASSIGN
%token PLUS MINUS MULT DIV
%token EQ NEQ LT GT LE GE
%token SEMI LPAREN RPAREN LBRACE RBRACE

/* Non-Terminal Types */
%type <nval> program statement_list statement block
%type <nval> expression equality comparison term factor primary

/* Start Rule */
%start program

%%

/* -------------------------------------------------------
   Grammar Rules
   ------------------------------------------------------- */

program:
    statement_list { root = $1; }
    ;

statement_list:
    statement_list statement { 
        /* Link the new statement to the end of the list */
        $$ = create_node_list($1, $2); 
    }
    | /* empty */ { $$ = NULL; }
    ;

statement:
    /* Variable Declaration: var x; */
    VAR IDENTIFIER SEMI { 
        add_symbol($2);            /* Record variable */
        $$ = create_var_decl($2, NULL); 
    }
    /* Variable Initialization: var x = 10; */
    | VAR IDENTIFIER ASSIGN expression SEMI { 
        add_symbol($2);            /* Record variable */
        $$ = create_var_decl($2, $4); 
    }
    /* Assignment: x = 10; */
    | IDENTIFIER ASSIGN expression SEMI { 
        check_symbol($1);          /* Verify it exists */
        $$ = create_assign($1, $3); 
    }
    /* If Statement */
    | IF LPAREN expression RPAREN statement { 
        $$ = create_if($3, $5, NULL); 
    }
    /* If-Else Statement */
    | IF LPAREN expression RPAREN statement ELSE statement { 
        $$ = create_if($3, $5, $7); 
    }
    /* While Loop */
    | WHILE LPAREN expression RPAREN statement { 
        $$ = create_while($3, $5); 
    }
    /* Block: { stmt_list } */
    | block { $$ = $1; }
    ;

block:
    LBRACE statement_list RBRACE { $$ = $2; }
    ;

/* -------------------------------------------------------
   Expression Handling (Precedence Logic)
   ------------------------------------------------------- */

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
    primary { $$ = $1; }
    | factor MULT primary { $$ = create_binop("*", $1, $3); }
    | factor DIV primary { $$ = create_binop("/", $1, $3); }
    ;

primary:
    INTEGER { $$ = create_int_node($1); }
    /* Identifier used in expression */
    | IDENTIFIER { 
        check_symbol($1);          /* Verify it exists */
        $$ = create_id_node($1); 
    }
    | LPAREN expression RPAREN { $$ = $2; }
    ;

%%

/* Error Handling Function */
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", line_num, s);
}