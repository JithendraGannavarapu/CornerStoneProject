#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <vector>
#include <cstdint>

class VM {
public:
    VM(const std::vector<int32_t>& bytecode);
    void run();
    void printFinalStack();
    bool validAddress(int addr);

private:
    std::vector<int32_t> program;
    std::vector<int32_t> stack;
    int pc;
    bool running;

    int32_t pop();
    void execute(int32_t opcode);
};

#endif
