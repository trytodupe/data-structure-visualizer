#pragma once

#include "DataStructure.h"
#include <vector>

class DynamicArray : public DataStructure {
private:
    std::vector<int> elements;

public:
    DynamicArray() = default;
    virtual ~DynamicArray() = default;

    // DataStructure interface implementation
    std::string getType() const override {
        return "DynamicArray";
    }

    size_t size() const override {
        return elements.size();
    }

    bool empty() const override {
        return elements.empty();
    }

    void clear() override {
        elements.clear();
        recordOperation(BaseOperationType::CUSTOM, static_cast<int>(LinearOps::Type::DELETE),
                       {}, {}, "Clear all elements");
    }

    std::vector<int> getSupportedOperations() const override {
        return {
            static_cast<int>(LinearOps::Type::INIT),
            static_cast<int>(LinearOps::Type::INSERT),
            static_cast<int>(LinearOps::Type::DELETE)
        };
    }

    bool supportsOperation(int operationType) const override {
        auto supported = getSupportedOperations();
        return std::find(supported.begin(), supported.end(), operationType) != supported.end();
    }

    std::string getOperationName(int operationType) const override {
        switch(static_cast<LinearOps::Type>(operationType)) {
            case LinearOps::Type::INIT: return "Initialize";
            case LinearOps::Type::INSERT: return "Insert";
            case LinearOps::Type::DELETE: return "Delete";
            default: return "Unknown";
        }
    }

    bool initialize(const std::vector<int>& data) override {
        elements = data;
        recordOperation(BaseOperationType::INIT, static_cast<int>(LinearOps::Type::INIT),
                       {}, data, "Initialize array with data");
        return true;
    }

    std::vector<Operation> getOperationHistory() const override {
        return operationHistory;
    }

    void clearOperationHistory() override {
        operationHistory.clear();
    }

    // Array-specific operations
    bool insert(size_t index, int value) {
        if (index > elements.size()) {
            recordOperation(BaseOperationType::INSERT, static_cast<int>(LinearOps::Type::INSERT),
                           {static_cast<int>(index)}, {value}, 
                           "Insert failed: index out of bounds", false);
            return false;
        }
        
        elements.insert(elements.begin() + index, value);
        recordOperation(BaseOperationType::INSERT, static_cast<int>(LinearOps::Type::INSERT),
                       {static_cast<int>(index)}, {value}, 
                       "Insert " + std::to_string(value) + " at position " + std::to_string(index));
        return true;
    }

    bool deleteAt(size_t index) {
        if (index >= elements.size()) {
            recordOperation(BaseOperationType::DELETE, static_cast<int>(LinearOps::Type::DELETE),
                           {static_cast<int>(index)}, {}, 
                           "Delete failed: index out of bounds", false);
            return false;
        }
        
        int deletedValue = elements[index];
        elements.erase(elements.begin() + index);
        recordOperation(BaseOperationType::DELETE, static_cast<int>(LinearOps::Type::DELETE),
                       {static_cast<int>(index)}, {deletedValue}, 
                       "Delete element at position " + std::to_string(index));
        return true;
    }

    void pushBack(int value) {
        insert(elements.size(), value);
    }

    // Getters for visualization
    const std::vector<int>& getElements() const {
        return elements;
    }

    int at(size_t index) const {
        if (index >= elements.size()) {
            throw std::out_of_range("Index out of bounds");
        }
        return elements[index];
    }

    // Serialization
    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["metadata"]["type"] = getType();
        j["metadata"]["version"] = "1.0";
        j["data"]["elements"] = elements;
        j["data"]["size"] = elements.size();
        
        // Serialize operations
        j["operations"] = nlohmann::json::array();
        for (const auto& op : operationHistory) {
            nlohmann::json opJson;
            opJson["baseType"] = static_cast<int>(op.baseType);
            opJson["specificType"] = op.specificType;
            opJson["dataStructureType"] = op.dataStructureType;
            opJson["indices"] = op.indices;
            opJson["values"] = op.values;
            opJson["description"] = op.description;
            opJson["success"] = op.success;
            j["operations"].push_back(opJson);
        }
        
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        if (j["metadata"]["type"] != getType()) {
            throw std::invalid_argument("Invalid data structure type for deserialization");
        }
        
        elements = j["data"]["elements"].get<std::vector<int>>();
        
        // Deserialize operations if present
        if (j.contains("operations")) {
            operationHistory.clear();
            for (const auto& opJson : j["operations"]) {
                operationHistory.emplace_back(
                    static_cast<BaseOperationType>(opJson["baseType"].get<int>()),
                    opJson["specificType"].get<int>(),
                    opJson["dataStructureType"].get<std::string>(),
                    opJson["indices"].get<std::vector<int>>(),
                    opJson["values"].get<std::vector<int>>(),
                    opJson["description"].get<std::string>(),
                    opJson["success"].get<bool>()
                );
            }
        }
    }
};