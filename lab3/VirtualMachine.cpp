#include "VirtualMachine.h"
#include "Instruction.h"
#include <iostream>

using namespace std;

VM::VM(const vector<int32_t>& bytecode)
    : program(bytecode), pc(0), running(true) {}

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
    while (running) {
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

