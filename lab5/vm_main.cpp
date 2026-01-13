#include "VirtualMachine.h"
#include "Object.h"
#include "Value.h"
#include <iostream>

// GC functions
void gc_mark_from_vm(const VM& vm);
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

    // Mark phase
    gc_mark_from_vm(vm);

    // Results
    std::cout << "obj1 marked = " << obj1->marked << std::endl;
    std::cout << "obj2 marked = " << obj2->marked << std::endl;
    std::cout << "obj3 marked = " << obj3->marked << std::endl;

    return 0;
}
