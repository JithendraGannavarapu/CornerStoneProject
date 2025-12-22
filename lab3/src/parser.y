%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int line_num;
void yyerror(const char *s);

ASTNode* root; // The final tree
%}

/* Define the types for tokens and non-terminals */
%union {
    int ival;
    char* sval;
    struct ASTNode* nval;
}

%token <sval> IDENTIFIER
%token <ival> INTEGER
%token VAR IF ELSE WHILE
%token ASSIGN PLUS MINUS MULT DIV EQ NEQ LT GT LE GE
%token SEMI LPAREN RPAREN LBRACE RBRACE

/* Define non-terminal types */
%type <nval> program statement_list statement expression equality comparison term factor primary

%%

program:
    statement_list { root = $1; }
    ;

statement_list:
    statement_list statement { $$ = create_node_list($1, $2); }
    | /* empty */ { $$ = NULL; }
    ;

statement:
    VAR IDENTIFIER SEMI { $$ = create_var_decl($2, NULL); }
    | VAR IDENTIFIER ASSIGN expression SEMI { $$ = create_var_decl($2, $4); }
    | IDENTIFIER ASSIGN expression SEMI { $$ = create_assign($1, $3); }
    | IF LPAREN expression RPAREN statement { $$ = create_if($3, $5, NULL); }
    | IF LPAREN expression RPAREN statement ELSE statement { $$ = create_if($3, $5, $7); }
    | WHILE LPAREN expression RPAREN statement { $$ = create_while($3, $5); }
    | LBRACE statement_list RBRACE { $$ = $2; }
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
    | IDENTIFIER { $$ = create_id_node($1); }
    | LPAREN expression RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", line_num, s);
}