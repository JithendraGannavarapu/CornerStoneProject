#include "VirtualMachine.h"
#include "Instruction.h"
#include <iostream>

using namespace std;

// --- Constructor & Destructor ---
VM::VM(const vector<int32_t>& bytecode)
    : program(bytecode), 
      memory(1024),      // Default Value() is INT 0
      objects(nullptr),  // Heap starts empty
      instructionCount(0),
      maxStackDepth(0),
      pc(0), 
      running(true) {}

VM::~VM() {
    // Cleanup: Free all objects when VM dies
    Object* obj = objects;
    while (obj != nullptr) {
        Object* next = obj->next;
        delete obj;
        obj = next;
    }
}

// --- Status Helpers ---
int VM::getObjectCount() {
    int count = 0;
    Object* obj = objects;
    while (obj != nullptr) {
        count++;
        obj = obj->next;
    }
    return count;
}

void VM::printHeapStatus() {
    int count = getObjectCount();
    // Assuming ObjPair size for approximation
    cout << "   [Heap Status] Active Objects: " << count 
         << " | Bytes Used (approx): " << count * sizeof(ObjPair) << " bytes" << endl;
}

// --- Stack Helpers ---
void VM::push(Value v) {
    stack.push_back(v);
    updateMaxStackDepth();
}

void VM::pushStack(Value v) { // Public wrapper for tests
    push(v);
}

Value VM::pop() {
    if (stack.empty()) {
        cerr << "Stack underflow\n";
        running = false;
        return INT_VAL(0);
    }
    Value v = stack.back();
    stack.pop_back();
    return v;
}

// --- Lab 5: Allocator ---
Object* VM::allocatePair(Object* a, Object* b) {
    ObjPair* pair = new ObjPair();
    pair->obj.type = OBJ_PAIR;
    pair->obj.marked = false;
    
    // Add to the front of the "Heap" list
    pair->obj.next = objects;
    objects = (Object*)pair;

    pair->left = a;
    pair->right = b;
    return (Object*)pair;
}

// --- Lab 5: Garbage Collector ---

// 1. Mark Phase (Recursive)
void VM::markObject(Object* obj) {
    if (obj == nullptr || obj->marked) return;
    
    obj->marked = true; // Mark self

    // RECURSION: Check children
    if (obj->type == OBJ_PAIR) {
        ObjPair* pair = (ObjPair*)obj;
        markObject(pair->left);
        markObject(pair->right);
    }
}

void VM::markValue(Value v) {
    if (IS_OBJ(v)) markObject(AS_OBJ(v));
}

// 2. Sweep Phase (Returns count of freed objects)
int VM::sweep() {
    int freedCount = 0;
    Object** object = &objects;
    while (*object != nullptr) {
        if (!(*object)->marked) {
            // Unreached -> Garbage
            Object* unreached = *object;
            *object = unreached->next; // Unlink
            delete unreached;          // Free C++ memory
            freedCount++;
        } else {
            // Reached -> Reset mark for next cycle
            (*object)->marked = false;
            object = &(*object)->next;
        }
    }
    return freedCount;
}

// 3. GC Driver (Returns Stats)
GCStats VM::gc() {
    int initial = getObjectCount();

    // Mark Roots: Stack
    for (const Value& v : stack) markValue(v);
    
    // Mark Roots: Memory (Globals)
    for (const Value& v : memory) markValue(v);

    // Sweep
    int freed = sweep();
    
    GCStats stats;
    stats.initialCount = initial;
    stats.objectsFreed = freed;
    stats.objectsSurvived = initial - freed;
    return stats;
}

// --- Execution Engine ---
void VM::execute(int32_t opcode) {
    switch (opcode) {
        case OP_PUSH:
            push(INT_VAL(program[pc++]));
            break;

        case OP_ADD: {
            Value b = pop();
            Value a = pop();
            if (IS_INT(a) && IS_INT(b)) 
                push(INT_VAL(AS_INT(a) + AS_INT(b)));
            else {
                cerr << "Type Error: ADD expects integers\n";
                running = false;
            }
            break;
        }

        case OP_SUB: {
            Value b = pop();
            Value a = pop();
            if (IS_INT(a) && IS_INT(b)) 
                push(INT_VAL(AS_INT(a) - AS_INT(b)));
            else {
                cerr << "Type Error: SUB expects integers\n";
                running = false;
            }
            break;
        }

        case OP_MUL: {
            Value b = pop();
            Value a = pop();
            if (IS_INT(a) && IS_INT(b)) 
                push(INT_VAL(AS_INT(a) * AS_INT(b)));
            else {
                cerr << "Type Error: MUL expects integers\n";
                running = false;
            }
            break;
        }

        case OP_DIV: {
            Value b = pop();
            Value a = pop();
            if (IS_INT(a) && IS_INT(b)) {
                if (AS_INT(b) == 0) {
                    cerr << "Division by zero\n";
                    running = false;
                    break;
                }
                push(INT_VAL(AS_INT(a) / AS_INT(b)));
            } else {
                cerr << "Type Error: DIV expects integers\n";
                running = false;
            }
            break;
        }

        case OP_CMP: {
            Value b = pop();
            Value a = pop();
            if (IS_INT(a) && IS_INT(b)) 
                push(INT_VAL(AS_INT(a) < AS_INT(b) ? 1 : 0));
            else {
                cerr << "Type Error: CMP expects integers\n";
                running = false;
            }
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

        case OP_JZ: {
            int32_t addr = program[pc++];
            Value cond = pop();
            if (!IS_INT(cond)) {
                cerr << "JZ expects integer condition\n";
                running = false;
                break;
            }
            if (AS_INT(cond) == 0) pc = addr;
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
            if (validMemory(idx)) {
                memory[idx] = v; // Allow storing Objects
            } else {
                cerr << "Invalid STORE index\n";
                running = false;
            }
            break;
        }

        case OP_LOAD: {
            int32_t idx = program[pc++];
            if (validMemory(idx)) {
                push(memory[idx]); // Allow loading Objects
            } else {
                cerr << "Invalid LOAD index\n";
                running = false;
            }
            break;
        }

        case OP_DUP: {
             if (stack.empty()) {
                cerr << "DUP on empty stack\n";
                running = false;
                break;
             }
             push(stack.back());
             break;
        }

        case OP_POP: {
            pop();
            break;
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

        case OP_HALT:
            running = false;
            return;
            
        default:
            cerr << "Unknown opcode " << opcode << endl;
            running = false;
            break;
    }
    updateMaxStackDepth();
}

void VM::run() {
    int steps = 0;
    while (running && pc < (int)program.size()) {
        execute(program[pc++]);
        instructionCount++;
        if (++steps > 1000000) {
            cerr << "Possible infinite loop aborted.\n";
            break;
        }
    }
}

// --- Debugging ---
void VM::updateMaxStackDepth() {
    if (stack.size() > maxStackDepth) maxStackDepth = stack.size();
}

void VM::printFinalStack() {
    cout << "\n=== VM HALTED ===\n";
    if (stack.empty()) {
        cout << "Stack is empty\n";
    } else {
        cout << "Final Stack (bottom -> top): ";
        for (const Value& v : stack) {
            if (IS_INT(v)) cout << AS_INT(v) << " ";
            else cout << "<OBJ> ";
        }
        cout << endl;
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