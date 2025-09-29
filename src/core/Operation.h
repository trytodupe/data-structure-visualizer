#pragma once

#include <vector>
#include <string>
#include <chrono>

// Base operation types - extensible design
enum class BaseOperationType {
    INIT,           // Initialize with data set
    INSERT,         // Generic insertion
    DELETE,         // Generic deletion
    SEARCH,         // Search operation
    CUSTOM          // Custom operation (defined by subclass)
};

// Data structure specific operations
namespace LinearOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        INSERT = static_cast<int>(BaseOperationType::INSERT),
        DELETE = static_cast<int>(BaseOperationType::DELETE),
        // List-specific operations can be added here
    };
}

namespace StackOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        PUSH = static_cast<int>(BaseOperationType::INSERT),  // Push maps to insert
        POP = static_cast<int>(BaseOperationType::DELETE),   // Pop maps to delete
    };
}

namespace TreeOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        // Basic trees only support initialization
    };
}

namespace BSTOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        SEARCH = static_cast<int>(BaseOperationType::SEARCH),
        INSERT = static_cast<int>(BaseOperationType::INSERT),
        DELETE = static_cast<int>(BaseOperationType::DELETE),
    };
}

struct Operation {
    BaseOperationType baseType;      // Base operation type
    int specificType;                // Data structure specific type
    std::string dataStructureType;   // Which data structure this applies to
    std::vector<int> indices;        // Affected positions
    std::vector<int> values;         // Values involved
    std::string description;         // Human-readable description
    std::chrono::steady_clock::time_point time;  // When operation occurred
    bool success;                    // Whether operation succeeded

    // Constructor for convenience
    Operation(BaseOperationType base, int specific, const std::string& dsType,
              const std::vector<int>& idx, const std::vector<int>& vals,
              const std::string& desc, bool succeeded = true)
        : baseType(base), specificType(specific), dataStructureType(dsType),
          indices(idx), values(vals), description(desc),
          time(std::chrono::steady_clock::now()), success(succeeded) {}
};