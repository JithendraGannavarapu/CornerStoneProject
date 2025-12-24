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

struct ASTNode* create_unary(char* op, struct ASTNode* child) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_UNARY;
    node->val_str = strdup(op);
    node->left = child;
    node->right = NULL;
    node->next = NULL; 
    return node;
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
            printf("ASSIGNMENT: %s\n", node->name);
            print_ast(node->left, level + 1);
            break;
        case NODE_BINOP:
            printf("OPERATOR: %s\n", node->op);
            print_ast(node->left, level + 1);
            print_ast(node->right, level + 1);
            break;  
        case NODE_UNARY:
        printf("UNARY: %s\n", node->val_str);
        print_ast(node->left, level + 1);
        break;
        case NODE_INT:
            printf("VALUE: %d\n", node->value);
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
typedef struct {
    char name[50];
    int value;
} Memory;

Memory mem[1000];
int mem_count = 0;

int get_val(char* name) {
    for(int i=0; i<mem_count; i++) {
        if(strcmp(mem[i].name, name) == 0) return mem[i].value;
    }
    printf("Runtime Error: Variable %s not found\n", name);
    exit(1);
}

void set_val(char* name, int val) {
    for(int i=0; i<mem_count; i++) {
        if(strcmp(mem[i].name, name) == 0) {
            mem[i].value = val;
            return;
        }
    }
    strcpy(mem[mem_count].name, name);
    mem[mem_count].value = val;
    mem_count++;
}

int eval(ASTNode* node) {
    if (!node) return 0;

    if (node->type == NODE_INT) {
        return node->value;
    }
    else if (node->type == NODE_ID) {
        return get_val(node->name);
    }
    else if (node->type == NODE_UNARY) {
        int val = eval(node->left);
        if (strcmp(node->val_str, "-") == 0) return -val;
        return val; 
    }
    else if (node->type == NODE_BINOP) {
        int lhs = eval(node->left);
        int rhs = eval(node->right);
        if (strcmp(node->op, "+") == 0) return lhs + rhs;
        if (strcmp(node->op, "-") == 0) return lhs - rhs;
        if (strcmp(node->op, "*") == 0) return lhs * rhs;
        if (strcmp(node->op, "/") == 0) {
        if (rhs == 0) {
            fprintf(stderr, "Runtime Error: Division by zero detected!\n");
            exit(1);
        }
            return lhs / rhs;
        }
        if (strcmp(node->op, "==") == 0) return lhs == rhs;
        if (strcmp(node->op, "!=") == 0) return lhs != rhs;
        if (strcmp(node->op, "<") == 0) return lhs < rhs;
        if (strcmp(node->op, ">") == 0) return lhs > rhs;
        if (strcmp(node->op, "<=") == 0) return lhs <= rhs;
        if (strcmp(node->op, ">=") == 0) return lhs >= rhs;
    }
    return 0;
}

void exec(ASTNode* node) {
    if (!node) return;

    if (node->type == NODE_VAR_DECL) {
        int val = 0;
        if (node->left) val = eval(node->left);
        set_val(node->name, val);
        printf(">> Executed: var %s = %d\n", node->name, val);
    }
    else if (node->type == NODE_ASSIGN) {
        int val = eval(node->left);
        set_val(node->name, val);
        printf(">> Executed: %s = %d\n", node->name, val);
    }
    else if (node->type == NODE_IF) {
        if (eval(node->condition)) {
            exec(node->body);
        } else if (node->else_body) {
            exec(node->else_body);
        }
    }
    else if (node->type == NODE_WHILE) {
        while (eval(node->condition)) {
            exec(node->body);
        }
    }
    if (node->next) exec(node->next);
}

void print_output(ASTNode* root) {
    printf("\n--- EXECUTION OUTPUT ---\n");
    exec(root);
    printf("------------------------\n");
}
