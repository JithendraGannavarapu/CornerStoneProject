#ifndef AST_H
#define AST_H

typedef enum {
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_BINOP,
    NODE_INT,
    NODE_ID,
    NODE_STMT_LIST
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char* name;                 // For identifiers
    int value;                  // For integer literals
    char* op;                   // For operators like "+", "=="
    struct ASTNode *left;       // Generic child 1
    struct ASTNode *right;      // Generic child 2
    struct ASTNode *condition;  // For if/while
    struct ASTNode *body;       // For loop/if bodies
    struct ASTNode *else_body;  // For if-else
    struct ASTNode *next;       // To link statements in a list
} ASTNode;

// Function prototypes
ASTNode* create_int_node(int val);
ASTNode* create_id_node(char* name);
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right);
ASTNode* create_var_decl(char* name, ASTNode* init);
ASTNode* create_assign(char* name, ASTNode* expr);
ASTNode* create_if(ASTNode* cond, ASTNode* body, ASTNode* else_body);
ASTNode* create_while(ASTNode* cond, ASTNode* body);
ASTNode* create_node_list(ASTNode* list, ASTNode* stmt);

void print_ast(ASTNode* node, int level);

#endif