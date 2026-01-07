#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>
using namespace std;
class VM {
public:
    VM(const std::vector<int32_t>& bytecode);
    void run();
    void printFinalStack();
    bool validAddress(int addr);
    bool validMemory(int idx);
    void printStats();

private:
    vector<int32_t> program;
    vector<int32_t> stack;
    vector<int32_t> memory;
    vector<int32_t> callStack;
    long long instructionCount;
    size_t maxStackDepth;

    int pc;
    bool running;

    int32_t pop();
    void execute(int32_t opcode);
    void updateMaxStackDepth();

};


#endif
