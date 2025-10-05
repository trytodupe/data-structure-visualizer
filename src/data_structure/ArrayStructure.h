#pragma once
#include "../core/DataStructure.h"
#include <vector>
#include <string>
#include <sstream>

/**
 * Array data structure implementation.
 * Provides basic array operations with fixed or dynamic sizing.
 */
class ArrayStructure : public DataStructure {
public:
    std::vector<int> data;

    ArrayStructure() = default;

    void initialize() override {
        data.clear();
    }

    std::string getType() const override {
        return "Array";
    }

    std::string getStateDescription() const override {
        std::ostringstream oss;
        oss << "Array[" << data.size() << "]: [";
        for (size_t i = 0; i < data.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << data[i];
        }
        oss << "]";
        return oss.str();
    }
};
