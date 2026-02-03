#include "VirtualMachine.h"
#include "Instruction.h"
#include <iostream>

using namespace std;

VM::VM(const vector<int32_t>& bytecode)
    : program(bytecode), 
      memory(1024, 0), 
      instructionCount(0),
      maxStackDepth(0),
      pc(0), 
      running(true) {}

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
            updateMaxStackDepth();
            break;

        case OP_ADD:{ 
            int32_t b = pop();
            if (!running) break; 
            int32_t a = pop();
            if (!running) break;
            stack.push_back(a + b);
            updateMaxStackDepth();
            break;
        }

        case OP_SUB: {
            int32_t b = pop();
            if (!running) break; 
            int32_t a = pop();
            if (!running) break;
            stack.push_back(a - b);
            updateMaxStackDepth();
            break;
        }

        case OP_MUL: {
            int32_t b = pop();
            if (!running) break; 
            int32_t a = pop();
            if (!running) break;
            stack.push_back(a * b);
            updateMaxStackDepth();
            break;
        }

        case OP_DIV: {
            int32_t b = pop();
            if (!running) break; 
            int32_t a = pop();
            if (!running) break;
            if (b == 0) {
                cerr << "Division by zero\n";
                running = false;
                break;
            }
            stack.push_back(a / b);
            updateMaxStackDepth();
            break;
        }

        case OP_CMP: {
            int32_t b = pop();
            if (!running) break; 
            int32_t a = pop();
            if (!running) break;
            stack.push_back(a < b ? 1 : 0);
            updateMaxStackDepth();
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
            updateMaxStackDepth();
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
            return;
        }   

        case OP_CALL: {
            int32_t addr = program[pc++];
            if (!validAddress(addr)) {
                cerr << "Invalid CALL address\n";
                running = false;
                break;
            }

            callStack.push_back(pc);  
            pc = addr;                
            break;
        }
        
        case OP_RET: {
            if (callStack.empty()) {
                cerr << "RET with empty call stack\n";
                running = false;
                break;
            }

            pc = callStack.back();
            callStack.pop_back();
            break;
        }

        default:
            cerr << "Unknown opcode " << opcode << " at PC = " << (pc - 1) << 
             ". Possible invalid jump target." << endl;
            running = false;

    }
}

void VM::run() {
    int steps = 0;
    const int MAX_STEPS = 1000000;
    while (running) {
        if (pc < 0 || static_cast<size_t>(pc) >= program.size()) {
            cerr << "PC out of bounds: " << pc << endl;
            break;
        }
        int32_t opcode = program[pc];
        pc++;                    
        instructionCount++;      
        execute(opcode);

        if (++steps > MAX_STEPS) {
            cerr << "Execution aborted: possible infinite loop\n";
            break;
        }
    }
}

void VM::updateMaxStackDepth() {
    if (stack.size() > maxStackDepth) {
        maxStackDepth = stack.size();
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
void VM::printStats() {
    cout << "\n=== Execution Statistics ===\n";
    cout << "Instructions executed: " << instructionCount << endl;
    cout << "Max stack depth: " << maxStackDepth << endl;
}

bool VM::validAddress(int addr) {
    return addr >= 0 && static_cast<size_t>(addr) < program.size();
}

bool VM::validMemory(int idx) {
    return idx >= 0 && static_cast<size_t>(idx) < memory.size();
}

