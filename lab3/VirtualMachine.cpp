#include <iostream>
#include <vector>
#include <cstdint>
using namespace std;
class VM{
private:
    vector<int32_t> program;  
    int pc;                        
    bool running;

public:
    VM(const std::vector<int32_t>& bytecode)
        : program(bytecode), pc(0), running(true) {}

    void run() {
        while (running) {
            int32_t instruction = fetch();
            execute(instruction);
        }
    }

private:
    int32_t fetch() {
        return program[pc++];
    }

    void execute(int32_t opcode) {
        switch (opcode) {
            case OP_HALT:
                running = false;
                break;

            default:
                cerr << "Unknown opcode: " << opcode << std::endl;
                running = false;
        }
    }
};
