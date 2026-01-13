#include "Object.h"
#include "Value.h"
#include "VirtualMachine.h"

Object* heap = nullptr;

void gc_mark_object(Object* obj) {
    if (!obj || obj->marked) return;
    obj->marked = true;
}

void gc_mark_from_vm(const VM& vm) {
    for (const Value& v : vm.getStack()) {
        if (IS_OBJ(v)) {
            gc_mark_object(AS_OBJ(v));
        }
    }
}

#include <iostream>

void gc_sweep() {
    Object** obj = &heap;

    while (*obj != nullptr) {
        if (!(*obj)->marked) {
            // Garbage object → remove from heap
            Object* unreached = *obj;
            *obj = unreached->next;

            delete unreached;   // free memory
        } else {
            // Live object → unmark for next GC cycle
            (*obj)->marked = false;
            obj = &((*obj)->next);
        }
    }
}
void gc_collect(const VM& vm) {
    gc_mark_from_vm(vm);
    gc_sweep();
}

