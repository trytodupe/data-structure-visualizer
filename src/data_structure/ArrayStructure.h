#pragma once
#include "../core/DataStructure.h"
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>

/**
 * Array data structure implementation using C-style array.
 * Provides basic array operations with fixed maximum capacity.
 */
class ArrayStructure : public DataStructure {
private:
    static const size_t MAX_SIZE = 100;
    int data[MAX_SIZE];
    size_t currentSize;

public:
    ArrayStructure() : currentSize(0) {
        std::memset(data, 0, sizeof(data));
    }

    void initialize() override {
        currentSize = 0;
        std::memset(data, 0, sizeof(data));
    }

    std::string getType() const override {
        return "Array";
    }

    std::string getStateDescription() const override {
        std::ostringstream oss;
        oss << "Array[" << currentSize << "/" << MAX_SIZE << "]: [";
        for (size_t i = 0; i < currentSize; ++i) {
            if (i > 0) oss << ", ";
            oss << data[i];
        }
        oss << "]";
        return oss.str();
    }

    // Helper methods to maintain interface compatibility
    size_t size() const { return currentSize; }
    size_t capacity() const { return MAX_SIZE; }

    int& operator[](size_t index) { return data[index]; }
    const int& operator[](size_t index) const { return data[index]; }

    void resize(size_t newSize) {
        if (newSize > MAX_SIZE) newSize = MAX_SIZE;
        if (newSize > currentSize) {
            // Zero out new elements
            std::memset(data + currentSize, 0, (newSize - currentSize) * sizeof(int));
        }
        currentSize = newSize;
    }

    int* begin() { return data; }
    const int* begin() const { return data; }
    int* end() { return data + currentSize; }
    const int* end() const { return data + currentSize; }
};
