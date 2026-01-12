#include "Object.h"
#include <cstdlib>

/* Simple heap list for Step 3 (no GC yet) */
static Object* heap = nullptr;

/* Allocate a pair object */
Object* alloc_pair(Object* left, Object* right) {
    ObjPair* pair = (ObjPair*)malloc(sizeof(ObjPair));

    pair->obj.type = OBJ_PAIR;
    pair->obj.marked = false;

    pair->obj.next = heap;
    heap = &pair->obj;

    pair->left = left;
    pair->right = right;

    return &pair->obj;
}
