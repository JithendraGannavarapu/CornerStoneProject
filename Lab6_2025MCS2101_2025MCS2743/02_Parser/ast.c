#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = type;
    node->left = node->right = node->condition = node->body = node->else_body = node->next = NULL;
    node->name = node->op = node->val_str = NULL;
    node->value = 0;
    node->val_int = 0;
    return node;
}

ASTNode* create_int_node(int val) {
    ASTNode* node = create_node(NODE_INT);
    if (node) node->value = val;
    return node;
}

ASTNode* create_id_node(char* name) {
    ASTNode* node = create_node(NODE_ID);
    if (node && name) node->name = strdup(name);
    return node;
}

ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_BINOP);
    if (node && op) node->op = strdup(op);
    if (node) {
        node->left = left;
        node->right = right;
    }
    return node;
}

ASTNode* create_unary(char* op, ASTNode* child) {
    ASTNode* node = create_node(NODE_UNARY);
    if (node && op) node->op = strdup(op);
    if (node) node->left = child;
    return node;
}

ASTNode* create_var_decl(char* name, ASTNode* init) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    if (node && name) node->name = strdup(name);
    if (node) node->left = init; 
    return node;
}

ASTNode* create_assign(char* name, ASTNode* expr) {
    ASTNode* node = create_node(NODE_ASSIGN);
    if (node && name) node->name = strdup(name);
    if (node) node->left = expr;
    return node;
}

ASTNode* create_if(ASTNode* cond, ASTNode* body, ASTNode* else_body) {
    ASTNode* node = create_node(NODE_IF);
    if (node) {
        node->condition = cond;
        node->body = body;
        node->else_body = else_body;
    }
    return node;
}

ASTNode* create_while(ASTNode* cond, ASTNode* body) {
    ASTNode* node = create_node(NODE_WHILE);
    if (node) {
        node->condition = cond;
        node->body = body;
    }
    return node;
}

ASTNode* create_node_list(ASTNode* list, ASTNode* stmt) {
    ASTNode* node = create_node(NODE_STMT_LIST);
    if (node) {
        node->left = list;
        node->right = stmt;
    }
    return node;
}

void print_indent(int level) {
    for (int i = 0; i < level; i++) printf("  ");
}



void print_ast(ASTNode* node, int level) {
    if (!node) return;
    switch (node->type) {
        case NODE_STMT_LIST:
            if (node->left) print_ast(node->left, level);
            if (node->right) print_ast(node->right, level);
            break;
        case NODE_VAR_DECL:
            print_indent(level);
            printf("VAR_DECL: %s\n", node->name ? node->name : "NULL");
            print_ast(node->left, level + 1);
            break;
        case NODE_ASSIGN:
            print_indent(level);
            printf("ASSIGNMENT: %s\n", node->name ? node->name : "NULL");
            print_ast(node->left, level + 1);
            break;
        case NODE_BINOP:
            print_indent(level);
            printf("OPERATOR: %s\n", node->op ? node->op : "NULL");
            print_ast(node->left, level + 1);
            print_ast(node->right, level + 1);
            break;  
        case NODE_UNARY:
            print_indent(level);
            printf("UNARY: %s\n", node->op ? node->op : "NULL");
            print_ast(node->left, level + 1);
            break;
        case NODE_INT:
            print_indent(level);
            printf("VALUE: %d\n", node->value);
            break;
        case NODE_ID:
            print_indent(level);
            printf("ID: %s\n", node->name ? node->name : "NULL");
            break;
        case NODE_IF:
            print_indent(level);
            printf("IF\n");
            print_indent(level + 1); printf("CONDITION:\n");
            print_ast(node->condition, level + 2);
            print_indent(level + 1); printf("THEN:\n");
            print_ast(node->body, level + 2);
            if (node->else_body) {
                print_indent(level + 1); printf("ELSE:\n");
                print_ast(node->else_body, level + 2);
            }
            break;
        case NODE_WHILE:
            print_indent(level);
            printf("WHILE\n");
            print_indent(level + 1); printf("CONDITION:\n");
            print_ast(node->condition, level + 2);
            print_indent(level + 1); printf("BODY:\n");
            print_ast(node->body, level + 2);
            break;
        default:
            print_indent(level);
            printf("UNKNOWN NODE TYPE\n");
            break;
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->condition);
    free_ast(node->body);
    free_ast(node->else_body);
    free_ast(node->next);
    if (node->name) free(node->name);
    if (node->op) free(node->op);
    if (node->val_str) free(node->val_str);
    free(node);
}

void print_output(ASTNode* root) {
    (void)root;
    printf("\n--- SYSTEM EXECUTION ---\n");
    printf("Execution is handled by the Virtual Machine in Lab 6.\n");
    printf("------------------------\n");
}