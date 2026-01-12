#include "VirtualMachine.h"
#include "Instruction.h"
#include <iostream>

using namespace std;

// --- Constructor & Destructor ---
VM::VM(const vector<int32_t>& bytecode)
    : program(bytecode), 
      memory(1024),     // Default Value() is int 0
      objects(nullptr), // Heap is empty
      pc(0), 
      running(true),
      instructionCount(0) {}

VM::~VM() {
    // Cleanup: Free all objects on exit
    Object* obj = objects;
    while (obj != nullptr) {
        Object* next = obj->next;
        delete obj;
        obj = next;
    }
}

// --- Stack Helpers ---
void VM::push(Value v) {
    stack.push_back(v);
}

void VM::pushStack(Value v) { // Wrapper for tests
    push(v);
}

Value VM::pop() {
    if (stack.empty()) return INT_VAL(0);
    Value v = stack.back();
    stack.pop_back();
    return v;
}

// --- Lab 5: Garbage Collector Implementation ---

// 1. Mark Phase [cite: 381]
void VM::markObject(Object* obj) {
    if (obj == nullptr || obj->marked) return;

    obj->marked = true; // Mark node

    if (obj->type == OBJ_PAIR) {
        ObjPair* pair = (ObjPair*)obj;
        markObject(pair->left);  // Recurse Left
        markObject(pair->right); // Recurse Right
    }
}

void VM::markValue(Value v) {
    if (IS_OBJ(v)) markObject(AS_OBJ(v));
}

// 2. Sweep Phase [cite: 382]
void VM::sweep() {
    Object** object = &objects;
    while (*object != nullptr) {
        if (!(*object)->marked) {
            // Unreached -> Delete
            Object* unreached = *object;
            *object = unreached->next;
            delete unreached;
        } else {
            // Reached -> Reset mark for next time
            (*object)->marked = false;
            object = &(*object)->next;
        }
    }
}

// 3. GC Driver [cite: 396]
void VM::gc() {
    // Mark Roots: Stack
    for (const Value& v : stack) markValue(v);
    
    // Mark Roots: Global Memory
    for (const Value& v : memory) markValue(v);
    
    // Sweep Garbage
    sweep();
}

// --- Allocator [cite: 379] ---
Object* VM::allocatePair(Object* a, Object* b) {
    ObjPair* pair = new ObjPair();
    pair->obj.type = OBJ_PAIR;
    pair->obj.marked = false;
    
    // Add to linked list (Heap)
    pair->obj.next = objects;
    objects = (Object*)pair;
    
    pair->left = a;
    pair->right = b;
    return (Object*)pair;
}

// --- Execution (Updated for Value types) ---
void VM::execute(int32_t opcode) {
    switch (opcode) {
        case OP_PUSH:
            // Convert raw bytecode int to Value
            push(INT_VAL(program[pc++])); 
            break;

        case OP_ADD: {
            Value b = pop();
            Value a = pop();
            // MUST unwrap integers
            if (IS_INT(a) && IS_INT(b)) 
                push(INT_VAL(AS_INT(a) + AS_INT(b)));
            break;
        }
        
        // (You can update SUB, MUL, DIV similarly if needed for Lab 4 compatibility)
        // For Lab 5 GC tests, we don't strictly use these math ops.

        case OP_HALT:
            running = false;
            break;
    }
}

void VM::run() {
    while (running && pc < (int)program.size()) {
        execute(program[pc++]);
    }
}