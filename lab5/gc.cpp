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
