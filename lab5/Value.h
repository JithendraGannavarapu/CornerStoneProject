#ifndef VALUE_H
#define VALUE_H

#include "Object.h"
#include <cstdint>

enum ValueType { VAL_INT, VAL_OBJ };

struct Value {
    ValueType type;
    union {
        int32_t asInt;
        Object* asObj;
    };
    
    // Constructors for safety
    Value() : type(VAL_INT), asInt(0) {}
    Value(ValueType t) : type(t), asInt(0) {}
};

// Macros for easy usage
#define IS_INT(v) ((v).type == VAL_INT)
#define IS_OBJ(v) ((v).type == VAL_OBJ)
#define AS_INT(v) ((v).asInt)
#define AS_OBJ(v) ((v).asObj)

// Helpers to create values
inline Value INT_VAL(int32_t val) {
    Value v(VAL_INT); v.asInt = val; return v;
}
inline Value OBJ_VAL(Object* obj) {
    Value v(VAL_OBJ); v.asObj = obj; return v;
}

#endif