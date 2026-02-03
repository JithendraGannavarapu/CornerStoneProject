#include "VirtualMachine.h"
#include "Instruction.h"
#include <iostream>

VM::VM(const std::vector<int32_t>& bytecode)
    : program(bytecode), 
      memory(1024, INT_VAL(0)),      
      objects(nullptr),  
      instructionCount(0),
      maxStackDepth(0),
      pc(0), 
      running(true) {}

VM::~VM() {
    Object* obj = objects;
    while (obj != nullptr) {
        Object* next = obj->next;
        delete obj;
        obj = next;
    }
}

int VM::getObjectCount() {
    int count = 0;
    Object* obj = objects;
    while (obj != nullptr) {
        count++;
        obj = obj->next;
    }
    return count;
}

void VM::printRegisters() {
    std::cout << "--- VM Register State ---" << std::endl;
    std::cout << "PC    : " << pc << std::endl;
    std::cout << "Stack : [ ";
    for (const auto& v : stack) {
        std::cout << AS_INT(v) << " ";
    }
    std::cout << "]" << std::endl;
}

void VM::setBreakpoint(int address) {
    breakpoints.insert(address);
    std::cout << "Breakpoint set at " << address << std::endl;
}

void VM::clearBreakpoint(int address) {
    breakpoints.erase(address);
    std::cout << "Breakpoint removed from " << address << std::endl;
}

void VM::executeNext() {
    if (!running || pc >= (int)program.size()) return;

    if (breakpoints.count(pc)) {
        std::cout << "Breakpoint hit at PC: " << pc << std::endl;
        return; 
    }

    execute(program[pc++]);
    instructionCount++;
}

void VM::run() {
    running = true;
    while (running && pc < (int)program.size()) {
        if (breakpoints.count(pc)) {
            std::cout << "Stopped at breakpoint: " << pc << std::endl;
            break;
        }
        execute(program[pc++]);
        instructionCount++;
    }
}

void VM::printHeapStatus() {
    int count = getObjectCount();
    std::cout << "Heap objects : " << count << std::endl;
}

void VM::push(Value v) {
    stack.push_back(v);
    updateMaxStackDepth();
}

void VM::pushStack(Value v) {
    push(v);
}

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

Object* VM::allocatePair(Object* a, Object* b) {
    ObjPair* pair = new ObjPair();
    pair->obj.type = OBJ_PAIR;
    pair->obj.marked = false;
    pair->obj.next = objects;
    objects = (Object*)pair;

    pair->left = a;
    pair->right = b;
    return (Object*)pair;
}

void VM::markObject(Object* obj) {
    if (obj == nullptr || obj->marked) return;
    obj->marked = true; 
    if (obj->type == OBJ_PAIR) {
        ObjPair* pair = (ObjPair*)obj;
        markObject(pair->left);
        markObject(pair->right);
    }
}

void VM::markValue(Value v) {
    if (IS_OBJ(v)) markObject(AS_OBJ(v));
}

int VM::sweep() {
    int freedCount = 0;
    Object** object = &objects;
    while (*object != nullptr) {
        if (!(*object)->marked) {
            Object* unreached = *object;
            *object = unreached->next; 
            delete unreached;          
            freedCount++;
        } else {
            (*object)->marked = false;
            object = &(*object)->next;
        }
    }
    return freedCount;
}

GCStats VM::gc() {
    int initial = getObjectCount();
    for (const Value& v : stack) markValue(v);
    for (const Value& v : memory) markValue(v);
    int freed = sweep();
    
    GCStats stats;
    stats.initialCount = initial;
    stats.objectsFreed = freed;
    stats.objectsSurvived = initial - freed;
    return stats;
}

void VM::execute(int32_t opcode) {
    switch (opcode) {
        case OP_PUSH:
            push(INT_VAL(program[pc++]));
            break;
        case OP_ADD: {
            Value b = pop(); Value a = pop();
            push(INT_VAL(AS_INT(a) + AS_INT(b)));
            break;
        }
        case OP_SUB: {
            Value b = pop(); Value a = pop();
            push(INT_VAL(AS_INT(a) - AS_INT(b)));
            break;
        }
        case OP_MUL: {
            Value b = pop(); Value a = pop();
            push(INT_VAL(AS_INT(a) * AS_INT(b)));
            break;
        }
        case OP_DIV: {
            Value b = pop(); Value a = pop();
            if (AS_INT(b) == 0) { std::cerr << "Division by zero\n"; running = false; break; }
            push(INT_VAL(AS_INT(a) / AS_INT(b)));
            break;
        }
        case OP_CMP: {
            Value b = pop(); Value a = pop();
            push(INT_VAL(AS_INT(a) < AS_INT(b) ? 1 : 0));
            break;
        }
        case OP_JMP:
            pc = program[pc++];
            break;
        case OP_JZ: {
            int32_t addr = program[pc++];
            if (AS_INT(pop()) == 0) pc = addr;
            break;
        }
        case OP_JNZ: {
            int32_t addr = program[pc++];
            if (AS_INT(pop()) != 0) pc = addr;
            break;
        }
        case OP_STORE: {
            int32_t idx = program[pc++];
            memory[idx] = pop();
            break;
        }
        case OP_LOAD:
            push(memory[program[pc++]]);
            break;
        case OP_DUP:
            push(stack.back());
            break;
        case OP_POP:
            pop();
            break;
        case OP_CALL: {
            int32_t addr = program[pc++];
            callStack.push_back(pc);
            pc = addr;
            break;
        }
        case OP_RET:
            pc = callStack.back();
            callStack.pop_back();
            break;
        case OP_PRINT:
            std::cout << "Output : " << AS_INT(pop()) << std::endl;
            break;
        case OP_HALT:
            running = false;
            break;
        default:
            running = false;
            break;
    }
    updateMaxStackDepth();
}

void VM::updateMaxStackDepth() {
    if (stack.size() > maxStackDepth) maxStackDepth = stack.size();
}

void VM::printFinalStack() {
    if (stack.empty()) return;
    std::cout << "Final Stack: ";
    for (const Value& v : stack) std::cout << AS_INT(v) << " ";
    std::cout << std::endl;
}

std::string VM::getOpcodeName(int32_t opcode) {
    switch (opcode) {
        case 0x01: return "PUSH ";
        case 0x02: return "POP  ";
        case 0x03: return "DUP  ";
        case 0x10: return "ADD  ";
        case 0x11: return "SUB  ";
        case 0x12: return "MUL  ";
        case 0x13: return "DIV  ";
        case 0x14: return "CMP  ";
        case 0x20: return "JMP  ";
        case 0x21: return "JZ   ";
        case 0x22: return "JNZ  ";
        case 0x30: return "STORE";
        case 0x31: return "LOAD ";
        case 0x40: return "CALL ";
        case 0x41: return "RET  ";
        case 0x42: return "PRINT";
        case 0xFF: return "HALT ";
        default:   return "UNKNOWN";
    }
}

void VM::printStats() {
    std::cout << "\n=== Execution Statistics ===\n";
    std::cout << "Instructions executed: " << instructionCount << std::endl;
    std::cout << "Max stack depth: " << maxStackDepth << std::endl;
}

bool VM::validAddress(int addr) {
    return addr >= 0 && static_cast<size_t>(addr) < program.size();
}

bool VM::validMemory(int idx) {
    return idx >= 0 && static_cast<size_t>(idx) < memory.size();
}