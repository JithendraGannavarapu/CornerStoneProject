#ifndef OBJECT_H
#define OBJECT_H

enum ObjectType {
    OBJ_PAIR 
};

struct Object {
    ObjectType type;
    bool marked;       
    struct Object* next; 
};

struct ObjPair {
    Object obj;        
    Object* left;       
    Object* right;      
};

#endif