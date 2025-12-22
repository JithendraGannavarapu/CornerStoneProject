#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->condition = NULL;
    node->body = NULL;
    node->else_body = NULL;
    node->next = NULL;
    return node;
}

ASTNode* create_int_node(int val) {
    ASTNode* node = create_node(NODE_INT);
    node->value = val;
    return node;
}

ASTNode* create_id_node(char* name) {
    ASTNode* node = create_node(NODE_ID);
    node->name = strdup(name);
    return node;
}

ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_BINOP);
    node->op = strdup(op);
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* create_var_decl(char* name, ASTNode* init) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    node->name = strdup(name);
    node->left = init; 
    return node;
}

ASTNode* create_assign(char* name, ASTNode* expr) {
    ASTNode* node = create_node(NODE_ASSIGN);
    node->name = strdup(name);
    node->left = expr;
    return node;
}

ASTNode* create_if(ASTNode* cond, ASTNode* body, ASTNode* else_body) {
    ASTNode* node = create_node(NODE_IF);
    node->condition = cond;
    node->body = body;
    node->else_body = else_body;
    return node;
}

ASTNode* create_while(ASTNode* cond, ASTNode* body) {
    ASTNode* node = create_node(NODE_WHILE);
    node->condition = cond;
    node->body = body;
    return node;
}

ASTNode* create_node_list(ASTNode* list, ASTNode* stmt) {
    if (list == NULL) return stmt;
    ASTNode* temp = list;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = stmt;
    return list;
}

void print_indent(int level) {
    for (int i = 0; i < level; i++) printf("  ");
}

void print_ast(ASTNode* node, int level) {
    if (!node) return;

    print_indent(level);
    
    switch (node->type) {
        case NODE_VAR_DECL:
            printf("VAR_DECL: %s\n", node->name);
            print_ast(node->left, level + 1);
            break;
        case NODE_ASSIGN:
            printf("ASSIGN: %s\n", node->name);
            print_ast(node->left, level + 1);
            break;
        case NODE_BINOP:
            printf("OP: %s\n", node->op);
            print_ast(node->left, level + 1);
            print_ast(node->right, level + 1);
            break;
        case NODE_INT:
            printf("INT: %d\n", node->value);
            break;
        case NODE_ID:
            printf("ID: %s\n", node->name);
            break;
        case NODE_IF:
            printf("IF\n");
            print_indent(level+1); printf("CONDITION:\n");
            print_ast(node->condition, level + 2);
            print_indent(level+1); printf("THEN:\n");
            print_ast(node->body, level + 2);
            if (node->else_body) {
                print_indent(level+1); printf("ELSE:\n");
                print_ast(node->else_body, level + 2);
            }
            break;
        case NODE_WHILE:
            printf("WHILE\n");
            print_indent(level+1); printf("CONDITION:\n");
            print_ast(node->condition, level + 2);
            print_indent(level+1); printf("BODY:\n");
            print_ast(node->body, level + 2);
            break;
    }
    
    if (node->next) {
        print_ast(node->next, level);
    }
}