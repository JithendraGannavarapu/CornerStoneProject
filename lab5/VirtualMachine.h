#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>
#include "Value.h"
using namespace std;
class VM {
public:

    VM(const std::vector<int32_t>& bytecode);
    void run();
    const std::vector<Value>& getStack() const { return stack; }
    void printFinalStack();
    void printStats();

private:

    vector<int32_t> program;
    vector<Value> stack;
    vector<int32_t> memory;
    vector<int32_t> callStack;

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
};

#endif
