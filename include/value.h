#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include <vector>
#include <cstdio>

// Foward Declaration
struct ValueArray;

// Will be using double for all values
using Value = double;

struct ValueArray {
    std::vector<Value> values;
    ValueArray() = default;
    ~ValueArray() = default;

    void write(Value value);
    void free();
};

// Value printing
void printValue(Value value);

#endif
