#pragma once

#include "Operation.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <algorithm>

class DataStructure {
public:
    virtual ~DataStructure() = default;
    virtual std::string getType() const = 0;
    virtual size_t size() const = 0; 
    virtual bool empty() const = 0;
    virtual void clear() = 0;
    virtual nlohmann::json serialize() const = 0;
    virtual void deserialize(const nlohmann::json& j) = 0;
    virtual std::vector<Operation> getOperationHistory() const = 0;
    virtual void clearOperationHistory() = 0;

    // New methods for operation support
    virtual std::vector<int> getSupportedOperations() const = 0;  // Returns supported operation types
    virtual bool supportsOperation(int operationType) const = 0;  // Check if operation is supported
    virtual std::string getOperationName(int operationType) const = 0;  // Get human-readable name

    // Initialize with data set (required by all structures)
    virtual bool initialize(const std::vector<int>& data) = 0;

protected:
    std::vector<Operation> operationHistory;

    // Helper method to record operations
    void recordOperation(BaseOperationType baseType, int specificType, 
                        const std::vector<int>& indices, const std::vector<int>& values,
                        const std::string& description, bool success = true) {
        operationHistory.emplace_back(baseType, specificType, getType(), 
                                    indices, values, description, success);
    }
};