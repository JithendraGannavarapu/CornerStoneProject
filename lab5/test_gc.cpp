#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip> 
#include "VirtualMachine.h"
#include "Object.h"
#include "Value.h"

using namespace std;

#define PUSH_OBJ(vm, obj) vm.pushStack(OBJ_VAL(obj))


void printReport(const string& phase, const GCStats& stats) {
    cout << "   [" << phase << "] ";
    cout << "Start: " << stats.initialCount 
         << " -> Freed: " << stats.objectsFreed 
         << " -> Survived: " << stats.objectsSurvived << endl;
}

void runTest(const string& name, void (*testFunc)()) {
    cout << "\n==================================================" << endl;
    cout << "TEST: " << name << endl;
    cout << "==================================================" << endl;
    
    try {
        testFunc();
        cout << ">>> RESULT: PASSED" << endl;
    } catch (const exception& e) {
        cout << ">>> RESULT: FAILED (Exception: " << e.what() << ")" << endl;
        exit(1);
    } catch (...) {
        cout << ">>> RESULT: CRASHED" << endl;
        exit(1);
    }
}

void testBasicReachability() {
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    PUSH_OBJ(vm, a);
    
    vm.printHeapStatus();
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    vm.printHeapStatus();
    
    assert(stats.objectsSurvived == 1);
    assert(stats.objectsFreed == 0);
    cout << "   [Check] Object survived as expected." << endl;
}

void testUnreachable() {
    VM vm({});
    vm.allocatePair(nullptr, nullptr); 
    
    vm.printHeapStatus();
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    vm.printHeapStatus();

    assert(stats.objectsFreed == 1);
    assert(stats.objectsSurvived == 0);
    cout << "   [Check] Unreachable object was correctly freed." << endl;
}

void testTransitive() {
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    Object* b = vm.allocatePair(a, nullptr); 
    
    PUSH_OBJ(vm, b); 
    
    vm.printHeapStatus();
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    
    assert(stats.objectsSurvived == 2);
    cout << "   [Check] Child object 'a' was transitively marked." << endl;
}

void testCycle() {
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    Object* b = vm.allocatePair(a, nullptr);

    ((ObjPair*)a)->right = b; 
    
    PUSH_OBJ(vm, a);
    
    vm.printHeapStatus();
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    
    assert(stats.objectsSurvived == 2);
    cout << "   [Check] Cycle detected and preserved." << endl;
}

void testDeepGraph() {
    VM vm({});
    Object* root = vm.allocatePair(nullptr, nullptr);
    Object* cur = root;

    cout << "   [Allocating] Creating 10,000 linked objects..." << endl;
    for (int i = 0; i < 10000; i++) {
        Object* next = vm.allocatePair(nullptr, nullptr);
        ((ObjPair*)cur)->right = next;
        cur = next;
    }

    PUSH_OBJ(vm, root);
    
    vm.printHeapStatus();
    cout << "   [Stress] Running GC on deep graph..." << endl;
    
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
 
    assert(stats.objectsSurvived == 10001);
    cout << "   [Check] Recursion depth handled successfully." << endl;
}

void testClosure() {
    VM vm({});
    Object* env = vm.allocatePair(nullptr, nullptr); 
    Object* fn = vm.allocatePair(nullptr, nullptr); 
    Object* closure = vm.allocatePair(fn, env);
    
    PUSH_OBJ(vm, closure);
    
    vm.printHeapStatus();
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    
    assert(stats.objectsSurvived == 3);
    cout << "   [Check] Closure kept environment alive." << endl;
}

void testStress() {
    VM vm({});
    cout << "   [Allocating] Creating 10,000 garbage objects..." << endl;
    for (int i = 0; i < 10000; i++) {
        vm.allocatePair(nullptr, nullptr);
    }
    
    vm.printHeapStatus();
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    vm.printHeapStatus();
    
    if (stats.objectsFreed == 10000 && vm.getObjectCount() == 0) {
        cout << "   [Check] Memory Leaks: NONE" << endl;
    } else {
        cout << "   [Check] FAILED: Memory Leak Detected!" << endl;
        exit(1);
    }
}


int main() {
    cout << "Starting Lab 5 Garbage Collector Test Suite..." << endl;

    runTest("1.6.1 Basic Reachability", testBasicReachability);
    runTest("1.6.2 Unreachable Objects", testUnreachable);
    runTest("1.6.3 Transitive Reachability", testTransitive);
    runTest("1.6.4 Cyclic References", testCycle);
    runTest("1.6.5 Deep Object Graph", testDeepGraph);
    runTest("1.6.6 Closure Capture", testClosure);
    runTest("1.6.7 Stress Allocation", testStress);

    cout << "\n--------------------------------------------------" << endl;
    cout << "SUMMARY: All 7 Tests Passed. GC Implementation Verified." << endl;
    cout << "--------------------------------------------------" << endl;
    return 0;
}