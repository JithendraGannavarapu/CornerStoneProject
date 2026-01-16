#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip> 
#include <chrono> 
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
    cout << "EDGE TEST: " << name << endl;
    cout << "==================================================" << endl;
    
    try {
        testFunc();
        cout << ">>> RESULT: PASSED" << endl;
    } catch (...) {
        cout << ">>> RESULT: FAILED" << endl;
        exit(1);
    }
}


void testOrphanedCycle() {
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    Object* b = vm.allocatePair(a, nullptr);
    ((ObjPair*)a)->right = b;
    
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    
    assert(stats.objectsFreed == 2);
    assert(stats.objectsSurvived == 0);
    cout << "   [Check] Orphaned cycle correctly identified as garbage." << endl;
}

void testDiamondGraph() {
    VM vm({});
    Object* d = vm.allocatePair(nullptr, nullptr);
    Object* b = vm.allocatePair(d, nullptr);
    Object* c = vm.allocatePair(d, nullptr);
    Object* root = vm.allocatePair(b, c);
    
    PUSH_OBJ(vm, root);
    
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    
    assert(stats.objectsSurvived == 4);
    cout << "   [Check] Shared child (Diamond pattern) preserved correctly." << endl;
}

void testSelfCycle() {
    VM vm({});
    Object* a = vm.allocatePair(nullptr, nullptr);
    ((ObjPair*)a)->left = a;
    
    PUSH_OBJ(vm, a);
    
    GCStats stats = vm.gc();
    printReport("GC Cycle", stats);
    
    assert(stats.objectsSurvived == 1);
    cout << "   [Check] Self-referencing object preserved." << endl;
}

void testPerformanceStress() {
    VM vm({});
    const int COUNT = 50000; 

    cout << "   [Allocating] Creating " << COUNT << " objects..." << endl;
    
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < COUNT; i++) {
        vm.allocatePair(nullptr, nullptr);
    }
  
    GCStats stats = vm.gc();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    printReport("Performance Run", stats);
    
    cout << "   [Metrics] Time Taken: " << duration << " ms" << endl;
    cout << "   [Metrics] Objects Freed: " << stats.objectsFreed << endl;
    
    if (duration > 0) {
        cout << "   [Metrics] Throughput: ~" << (COUNT / duration) << " objs/ms" << endl;
    } else {
        cout << "   [Metrics] Throughput: Too fast to measure (<1ms)" << endl;
    }

    if (stats.objectsFreed == COUNT) {
        cout << "   [Check] Memory Integrity: PERFECT (No Leaks)" << endl;
    } else {
        cout << "   [Check] FAILED: Memory Leaks detected!" << endl;
        exit(1);
    }
}

int main() {
    cout << "Starting ADVANCED EDGE & PERFORMANCE SUITE..." << endl;

    runTest("Orphaned Cycle", testOrphanedCycle);
    runTest("Diamond Graph", testDiamondGraph);
    runTest("Self-Reference", testSelfCycle);
    runTest("Performance Stress (50k Objects)", testPerformanceStress);

    cout << "\n--------------------------------------------------" << endl;
    cout << "SUMMARY: All Edge & Performance Tests Passed." << endl;
    cout << "--------------------------------------------------" << endl;
    return 0;
}