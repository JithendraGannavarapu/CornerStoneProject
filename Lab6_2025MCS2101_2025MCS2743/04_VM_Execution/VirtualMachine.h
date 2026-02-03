#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>
#include <iostream>
#include <string>
#include <set>
#include "Value.h"
#include "Object.h"

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
    void executeNext();
    void stop() { running = false; }
    GCStats gc();           
    Object* allocatePair(Object* a, Object* b);
    
    void pushStack(Value v);
    int getObjectCount();      
    void printHeapStatus();    
    static std::string getOpcodeName(int32_t opcode);
    void printFinalStack();
    void printStats();
    void printRegisters();
    
    void setBreakpoint(int address);
    void clearBreakpoint(int address);
    
    size_t getPC() const { return pc; }
    bool isRunning() const { return running; }

private:
    std::vector<int32_t> program;
    std::vector<Value> stack;
    std::vector<Value> memory;
    std::vector<int32_t> callStack;
    std::set<int> breakpoints;
    
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