#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>
#include <iostream> // Added for printing
#include "Value.h"
#include "Object.h"

using namespace std;


struct GCStats {
    int objectsFreed;
    int objectsSurvived;
    int initialCount;
};

class VM {
public:
    VM(const std::vector<int32_t>& bytecode);
    ~VM();

    void run();

    GCStats gc();           
    
    Object* allocatePair(Object* a, Object* b);
    void pushStack(Value v); 
    
    // Debugging / Status
    int getObjectCount();      
    void printHeapStatus();    

    // Lab 4 functions
    void printFinalStack();
    void printStats();

private:
    vector<int32_t> program;
    vector<Value> stack;
    vector<Value> memory;
    vector<int32_t> callStack;
    
    Object* objects; 

    long long instructionCount;
    size_t maxStackDepth;

    int pc;
    bool running;

    Value pop();
    void push(Value v);
    void execute(int32_t opcode);
    void updateMaxStackDepth();

    bool validAddress(int addr);
    bool validMemory(int idx);

    void markObject(Object* obj);
    void markValue(Value v);

    int sweep(); 
};

#endif