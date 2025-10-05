#pragma once
#include "../core/DataStructure.h"
#include <stack>
#include <vector>
#include <string>
#include <sstream>

/**
 * Stack data structure implementation.
 * Provides LIFO (Last-In-First-Out) operations.
 */
class StackStructure : public DataStructure {
public:
    std::stack<int> data;

    StackStructure() = default;

    void initialize() override {
        while (!data.empty()) {
            data.pop();
        }
    }

    std::string getType() const override {
        return "Stack";
    }

    std::string getStateDescription() const override {
        std::ostringstream oss;
        oss << "Stack[" << data.size() << "]";
        if (!data.empty()) {
            oss << " top=" << data.top();
        } else {
            oss << " (empty)";
        }
        return oss.str();
    }
};
