#ifndef AST_H
#define AST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_BINOP,
    NODE_UNARY,
    NODE_INT,
    NODE_ID,
    NODE_STMT_LIST
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char* name;                
    int value;                 
    char* op;    
    char* val_str;      
    int val_int;               
    struct ASTNode *left;      
    struct ASTNode *right;      
    struct ASTNode *condition;  
    struct ASTNode *body;       
    struct ASTNode *else_body;  
    struct ASTNode *next;       
} ASTNode;

ASTNode* create_int_node(int val);
ASTNode* create_id_node(char* name);
ASTNode* create_binop(char* op, ASTNode* left, ASTNode* right);
ASTNode* create_var_decl(char* name, ASTNode* init);
ASTNode* create_assign(char* name, ASTNode* expr);
ASTNode* create_if(ASTNode* cond, ASTNode* body, ASTNode* else_body);
ASTNode* create_while(ASTNode* cond, ASTNode* body);
ASTNode* create_node_list(ASTNode* list, ASTNode* stmt);
ASTNode* create_unary(char* op, struct ASTNode* child);
void print_ast(ASTNode* node, int level);
void print_output(ASTNode* root);
void free_ast(ASTNode* node);

extern ASTNode* root;

#ifdef __cplusplus
}
#endif

#endif