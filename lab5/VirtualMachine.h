#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>
#include "Value.h"
#include "Object.h"

class VM {
public:
    VM(const std::vector<int32_t>& bytecode);
    ~VM(); // Destructor needed to clean up heap

    void run();

    // --- Lab 5 New Features ---
    void gc();           // Trigger Garbage Collection
    
    // Allocator (creates new objects)
    Object* allocatePair(Object* a, Object* b);
    
    // Public wrappers for testing
    void pushStack(Value v);
    
private:
    std::vector<int32_t> program;
    
    // UPDATED: Stack and Memory now hold 'Value'
    std::vector<Value> stack;
    std::vector<Value> memory;
    
    std::vector<int32_t> callStack;
    
    // The Heap (Linked List Head)
    Object* objects; 

    int pc;
    bool running;
    long long instructionCount;

    // Internal Helpers
    Value pop();
    void push(Value v);
    void execute(int32_t opcode);
    
    // GC Internals
    void markObject(Object* obj);
    void markValue(Value v);
    void sweep();
};

#endif