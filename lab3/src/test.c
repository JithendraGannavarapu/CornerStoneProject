#include <stdio.h>

extern int yylex();
extern char* yytext;
extern int line_num;

int main() {
    int token;
    printf("--- Starting Lexer Test ---\n");
    while ((token = yylex())) {
        printf("Token: %d | Text: %s | Line: %d\n", token, yytext, line_num);
    }
    return 0;
}