#include "VirtualMachine.h"
#include "Object.h"
#include "Value.h"
#include <iostream>

void gc_collect(const VM& vm);
extern Object* heap;


int main() {
    std::vector<int32_t> dummy;
    VM vm(dummy);

    // Create heap objects
    Object* obj1 = new Object{OBJ_DUMMY, false, nullptr};
    Object* obj2 = new Object{OBJ_DUMMY, false, nullptr};
    Object* obj3 = new Object{OBJ_DUMMY, false, nullptr};

    // Heap list
    obj1->next = obj2;
    obj2->next = obj3;
    obj3->next = nullptr;
    heap = obj1;

    // Push roots
    vm.pushTestValue(OBJ_VAL(obj1));
    vm.pushTestValue(INT_VAL(42));

    gc_collect(vm);

    std::cout << "\nHeap after GC:\n";
    Object* cur = heap;
    int count = 0;
    while (cur) {
        std::cout << "Live object at " << cur << std::endl;
        cur = cur->next;
        count++;
    }
    std::cout << "Live object count = " << count << std::endl;


    return 0;
}
