#include <stdio.h>
#include "ast.h"

extern int yyparse();
extern ASTNode* root;

int main() {
    printf("Parsing input...\n");
    if (yyparse() != 0) {
        printf("Parse Failed.\n");
        return 0;
    }
    printf("Parse Successful! Printing AST:\n");
    print_ast(root, 0);
    return 0;
}