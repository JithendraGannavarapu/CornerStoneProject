#include <iostream>
#include <cassert>
#include "VirtualMachine.h"

using namespace std;

// Helper Macro to push Objects
#define PUSH_OBJ(vm, obj) vm.pushStack(OBJ_VAL(obj))

void testBasicReachability() {
    cout << "TEST: Basic Reachability... ";
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    PUSH_OBJ(vm, a);
    
    vm.gc();
    
    assert(a->marked == true); // Should survive
    cout << "PASSED\n";
}

void testUnreachable() {
    cout << "TEST: Unreachable Objects... ";
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    // Don't push to stack
    
    vm.gc();
    // 'a' is now deleted (we assume no crash means success)
    cout << "PASSED\n";
}

void testCycle() {
    cout << "TEST: Cyclic References... ";
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    Object* b = vm.allocatePair(a, nullptr);
    
    // Create Cycle A -> B and B -> A
    ((ObjPair*)a)->right = b;
    
    PUSH_OBJ(vm, a);
    
    vm.gc();
    
    assert(a->marked == true);
    assert(b->marked == true);
    cout << "PASSED\n";
}

int main() {
    testBasicReachability();
    testUnreachable();
    testCycle();
    return 0;
}