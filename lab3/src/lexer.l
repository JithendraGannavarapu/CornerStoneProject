%{
#include <stdio.h>
#include <string.h>
#include <y.tab.h>

int line_num = 1;
%}

%%

"var"       { return VAR; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }

[a-zA-Z_][a-zA-Z0-9_]* { return IDENTIFIER; }
[0-9]+                 { return INTEGER; }

"="         { return ASSIGN; }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return MULT; }
"/"         { return DIV; }
"<"         { return LT; }
">"         { return GT; }
";"         { return SEMI; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"=="        { return EQ; }
"!="        { return NEQ; }
"<="        { return LE; }
">="        { return GE; }
"//".* ; 
[ \t\r]+    ; /* Ignore whitespace [cite: 108] */
\n          { line_num++; }

"//".* ; /* Ignore comments  */

.           { printf("Error: Unknown char '%s' at line %d\n", yytext, line_num); }

%%

int yywrap() { return 1; }