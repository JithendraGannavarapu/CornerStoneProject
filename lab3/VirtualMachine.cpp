#include "VirtualMachine.h"
#include "Instruction.h"
#include <iostream>

using namespace std;

VM::VM(const vector<int32_t>& bytecode)
    : program(bytecode), pc(0), running(true), memory(1024, 0) {}

int32_t VM::pop() {
    if (stack.empty()) {
        cerr << "Stack underflow\n";
        running = false;
        return 0;
    }
    int32_t v = stack.back();
    stack.pop_back();
    return v;
}

void VM::execute(int32_t opcode) {
    switch (opcode) {
        case OP_PUSH:
            stack.push_back(program[pc++]);
            break;

        case OP_ADD:{ 
            int32_t b = pop();
            int32_t a = pop();
            stack.push_back(a + b);
            break;
        }

        case OP_SUB: {
            int32_t b = pop();
            int32_t a = pop();
            stack.push_back(a - b);
            break;
        }

        case OP_MUL: {
            int32_t b = pop();
            int32_t a = pop();
            stack.push_back(a * b);
            break;
        }

        case OP_DIV: {
            int32_t b = pop();
            int32_t a = pop();
            if (b == 0) {
                cerr << "Division by zero\n";
                running = false;
                break;
            }
            stack.push_back(a / b);
            break;
        }

        case OP_CMP: {
            int32_t b = pop();
            int32_t a = pop();
            stack.push_back(a < b ? 1 : 0);
            break;
        }
        
        case OP_JMP: {
        int32_t addr = program[pc++];
            if (!validAddress(addr)) {
                cerr << "Invalid JMP address\n";
                running = false;
                break;
            }
            pc = addr;
            break;
        }

        case OP_DUP: {
            if (stack.empty()) {
                cerr << "DUP on empty stack\n";
                running = false;
                break;
            }
            stack.push_back(stack.back());
            break;
        }

        case OP_POP: {
            pop();
            break;
        }

        case OP_JZ: {
            int32_t addr = program[pc++];
            int32_t cond = pop();  

            if (cond == 0) {
                if (!validAddress(addr)) {
                    cerr << "Invalid JZ address\n";
                    running = false;
                    break;
                }
                pc = addr;
            }
            break;
        }

        case OP_JNZ: {
            int32_t addr = program[pc++];
            int32_t cond = pop();

            if (cond != 0) {
                if (!validAddress(addr)) {
                    cerr << "Invalid JNZ address\n";
                    running = false;
                    break;
                }
                pc = addr;
            }
            break;
        }
        case OP_STORE: {
            int32_t idx = program[pc++];

            if (!validMemory(idx)) {
                cerr << "Invalid STORE index\n";
                running = false;
                break;
            }

            int32_t val = pop();
            memory[idx] = val;
            break;
        }

        case OP_LOAD: {
            int32_t idx = program[pc++];

            if (!validMemory(idx)) {
                cerr << "Invalid LOAD index\n";
                running = false;
                break;
            }

            stack.push_back(memory[idx]);
            break;
        }


        case OP_HALT: {
            running = false;
            printFinalStack();
            break;
        }   


        default:
            cerr << "Unknown opcode\n";
            running = false;
    }
}

void VM::run() {
    int steps = 0;
    const int MAX_STEPS = 1000000;
    while (running) {
        if (++steps > MAX_STEPS) {
            cerr << "Execution aborted: possible infinite loop\n";
            break;
        }
        execute(program[pc++]);
    }
}

void VM::printFinalStack() {
    cout << "\n=== VM HALTED ===\n";
    if (stack.empty()) {
        cout << "Stack is empty\n";
    } else {
        cout << "Final Stack (bottom → top): ";
        for (int32_t v : stack) {
            cout << v << " ";
        }
        cout << "\nTop of Stack (RESULT): " << stack.back() << endl;
    }
}

bool VM::validAddress(int addr) {
    return addr >= 0 && addr < program.size();
}

bool VM::validMemory(int idx) {
    return idx >= 0 && idx < memory.size();
}

