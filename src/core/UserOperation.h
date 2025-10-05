#pragma once
#include "Operation.h"
#include <vector>
#include <memory>
#include <string>

/**
 * Composite operation consisting of multiple atomic operations.
 * Represents a high-level user action that may involve several steps.
 */
class UserOperation {
public:
    std::vector<std::unique_ptr<Operation>> operations;

protected:
    std::string name;
    std::string description;

public:
    UserOperation(const std::string& opName, const std::string& opDescription)
        : name(opName), description(opDescription) {}

    virtual ~UserOperation() = default;

    /**
     * Execute all atomic operations in sequence
     * @param ds The data structure to modify
     */
    virtual void execute(DataStructure& ds) {
        for (auto& op : operations) {
            op->execute(ds);
        }
    }

    /**
     * Undo all atomic operations in reverse order
     * @param ds The data structure to restore
     */
    virtual void undo(DataStructure& ds) {
        for (auto it = operations.rbegin(); it != operations.rend(); ++it) {
            (*it)->undo(ds);
        }
    }

    /**
     * Draw all atomic operations
     * @param vis The visualizer to use for rendering
     */
    virtual void draw(GuiVisualizer& vis) {
        for (auto& op : operations) {
            op->draw(vis);
        }
    }

    /**
     * Serialize this user operation to JSON
     * @return JSON representation including all atomic operations
     */
    virtual nlohmann::json serialize() const {
        nlohmann::json j;
        j["name"] = name;
        j["description"] = description;
        j["operations"] = nlohmann::json::array();
        for (const auto& op : operations) {
            j["operations"].push_back(op->serialize());
        }
        return j;
    }

    /**
     * Deserialize this user operation from JSON
     * Note: This is a base implementation. Derived classes should override
     * to properly reconstruct their atomic operations.
     * @param j JSON object containing user operation data
     */
    virtual void deserialize(const nlohmann::json& j) {
        name = j["name"];
        description = j["description"];
        // Note: Atomic operations deserialization must be handled by derived classes
        // as they need to know the specific operation types
    }

    /**
     * Get the name of this user operation
     * @return Operation name
     */
    std::string getName() const { return name; }

    /**
     * Get the description of this user operation
     * @return Operation description
     */
    std::string getDescription() const { return description; }

    /**
     * Get the list of atomic operations
     * @return Vector of operation pointers
     */
    const std::vector<std::unique_ptr<Operation>>& getOperations() const {
        return operations;
    }

    /**
     * Add an atomic operation to this user operation
     * @param op The operation to add
     */
    void addOperation(std::unique_ptr<Operation> op) {
        operations.push_back(std::move(op));
    }

    /**
     * Get the number of atomic operations
     * @return Count of atomic operations
     */
    size_t getOperationCount() const {
        return operations.size();
    }

    /**
     * Clone this user operation
     * @return A deep copy of this user operation
     */
    virtual std::unique_ptr<UserOperation> clone() const {
        auto cloned = std::make_unique<UserOperation>(name, description);
        for (const auto& op : operations) {
            cloned->operations.push_back(op->clone());
        }
        return cloned;
    }
};
