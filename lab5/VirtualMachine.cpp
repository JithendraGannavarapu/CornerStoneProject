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

Value VM::pop() {
    if (stack.empty()) {
        std::cerr << "Stack underflow\n";
        running = false;
        return INT_VAL(0);
    }
    Value v = stack.back();
    stack.pop_back();
    return v;
}

void VM::push(Value v) {
    stack.push_back(v);
}


void VM::execute(int32_t opcode) {
    switch (opcode) {
        case OP_PUSH:

            push(INT_VAL(program[pc++]));
            updateMaxStackDepth();
            break;

        case OP_ADD:{ 
            Value b = pop();
            if (!running) break; 
            Value a = pop();
            if (!running) break; 

            if (!IS_INT(a) || !IS_INT(b)) {
                std::cerr << "ADD expects integers\n";
                running = false;
                return;
            }

            push(INT_VAL(AS_INT(a) + AS_INT(b)));

            updateMaxStackDepth();
            break;
        }

        case OP_SUB: {
            Value b = pop();
            if (!running) break; 
            Value a = pop();
            if (!running) break; 

            if (!IS_INT(a) || !IS_INT(b)) {
                std::cerr << "ADD expects integers\n";
                running = false;
                return;
            }

            push(INT_VAL(AS_INT(a) - AS_INT(b)));

            updateMaxStackDepth();
            break;
            break;
        }

        case OP_MUL: {
            Value b = pop();
            if (!running) break; 
            Value a = pop();
            if (!running) break; 

            if (!IS_INT(a) || !IS_INT(b)) {
                std::cerr << "ADD expects integers\n";
                running = false;
                return;
            }

            push(INT_VAL(AS_INT(a) * AS_INT(b)));

            updateMaxStackDepth();
            break;
        }


        case OP_DIV: {
            Value b = pop();
            if (!running) break; 
            Value a = pop();
            if (!running) break; 

            if (!IS_INT(a) || !IS_INT(b)) {
                std::cerr << "ADD expects integers\n";
                running = false;
                return;
            }

            if (AS_INT(b) == 0) {
                cerr << "Division by zero\n";
                running = false;
                break;
            }

            push(INT_VAL(AS_INT(a) / AS_INT(b)));
            updateMaxStackDepth();
            break;
        }

        case OP_CMP: {
            Value b = pop();
            if (!running) break;
            Value a = pop();
            if (!running) break;

            if (!IS_INT(a) || !IS_INT(b)) {
                cerr << "CMP expects integers\n";
                running = false;
                break;
            }

            push(INT_VAL(AS_INT(a) < AS_INT(b) ? 1 : 0));
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
                std::cerr << "DUP on empty stack\n";
                running = false;
                return;
            }
            push(stack.back());

            updateMaxStackDepth();
            break;
        }

        case OP_POP: {
            pop();
            break;
        }

        case OP_JZ: {
            int32_t addr = program[pc++];
            Value cond = pop();
            if (!IS_INT(cond)) {
                cerr << "JZ expects integer condition\n";
                running = false;
                break;
            }
            if (AS_INT(cond) == 0) {
                pc = addr;
            }
            break;
        }


        case OP_JNZ: {
            int32_t addr = program[pc++];
            Value cond = pop();
            if (!IS_INT(cond)) {
                cerr << "JNZ expects integer condition\n";
                running = false;
                break;
            }
            if (AS_INT(cond) != 0) {
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
            Value v = pop();
            if (!IS_INT(v)) {
                cerr << "STORE expects integer\n";
                running = false;
                break;
            }
            memory[idx] = AS_INT(v);
            break;
        }

        case OP_LOAD: {
            int32_t idx = program[pc++];
            push(INT_VAL(memory[idx]));
            updateMaxStackDepth();
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

        for (const Value& v : stack) {
            if (IS_INT(v))
                cout << AS_INT(v) << " ";
            else
                cout << "<obj> ";
        }
        if (IS_INT(stack.back()))
            cout << "Top of Stack (RESULT): " << AS_INT(stack.back()) << endl;
        else
            cout << "Top of Stack (RESULT): <obj>" << endl;

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

