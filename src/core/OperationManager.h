#pragma once
#include "Operation.h"
#include "UserOperation.h"
#include "DataStructure.h"
#include <vector>
#include <memory>
#include <stack>

/**
 * Manages operation history and provides undo/redo functionality.
 * Stores executed user operations and maintains undo/redo stacks.
 */
class OperationManager {
public:
    // Pair of operation and the data structure it was executed on
    struct OperationRecord {
        std::unique_ptr<UserOperation> operation;
        DataStructure* dataStructure;
    };

private:
    std::vector<std::unique_ptr<UserOperation>> executedOperations;
    std::vector<OperationRecord> undoStack;  // Use vector instead of stack for iteration
    std::vector<OperationRecord> redoStack;  // Use vector instead of stack for iteration

public:
    OperationManager() {}

    /**
     * Execute a user operation and add it to history
     * @param ds The data structure to execute on
     * @param userOp The user operation to execute
     * @param alreadyExecuted If true, operation was already executed (e.g., through stepping)
     */
    void executeOperation(DataStructure& ds, std::unique_ptr<UserOperation> userOp, bool alreadyExecuted = false) {
        if (!userOp) return;

        // Execute the user operation (unless already executed)
        if (!alreadyExecuted) {
            userOp->execute(ds);
        }

        // Clear redo stack when a new operation is executed
        redoStack.clear();

        // Add to undo stack (clone the user operation and store data structure pointer)
        OperationRecord record;
        record.operation = userOp->clone();
        record.dataStructure = &ds;
        undoStack.push_back(std::move(record));

        // Add to history
        executedOperations.push_back(std::move(userOp));
    }

    /**
     * Undo the last operation
     * @return true if undo was successful, false if nothing to undo
     */
    bool undo() {
        if (undoStack.empty()) {
            return false;
        }

        auto record = std::move(undoStack.back());
        undoStack.pop_back();

        // Use the stored data structure pointer
        record.operation->undo(*record.dataStructure);

        redoStack.push_back(std::move(record));

        return true;
    }

    /**
     * Redo the last undone operation
     * @return true if redo was successful, false if nothing to redo
     */
    bool redo() {
        if (redoStack.empty()) {
            return false;
        }

        auto record = std::move(redoStack.back());
        redoStack.pop_back();

        // Use the stored data structure pointer
        record.operation->execute(*record.dataStructure);

        undoStack.push_back(std::move(record));

        return true;
    }

    /**
     * Check if undo is available
     * @return true if there are operations to undo
     */
    bool canUndo() const {
        return !undoStack.empty();
    }

    /**
     * Check if redo is available
     * @return true if there are operations to redo
     */
    bool canRedo() const {
        return !redoStack.empty();
    }

    /**
     * Get executed operations vector
     * @return const reference to executed operations
     */
    const std::vector<std::unique_ptr<UserOperation>>& getExecutedOperations() const {
        return executedOperations;
    }

    /**
     * Get the number of executed operations in history
     * @return Count of operations
     */
    size_t getHistorySize() const {
        return executedOperations.size();
    }

    /**
     * Clear all operation history and stacks
     */
    void clear() {
        executedOperations.clear();
        undoStack.clear();
        redoStack.clear();
    }

    /**
     * Get the operation history
     * @return Vector of executed user operations
     */
    const std::vector<std::unique_ptr<UserOperation>>& getHistory() const {
        return executedOperations;
    }

    /**
     * Get the size of undo stack
     * @return Number of operations in undo stack
     */
    size_t getUndoStackSize() const {
        return undoStack.size();
    }

    /**
     * Get the size of redo stack
     * @return Number of operations in redo stack
     */
    size_t getRedoStackSize() const {
        return redoStack.size();
    }

    /**
     * Get const reference to undo stack
     * @return const reference to undo stack vector
     */
    const std::vector<OperationRecord>& getUndoStack() const {
        return undoStack;
    }

    /**
     * Get const reference to redo stack
     * @return const reference to redo stack vector
     */
    const std::vector<OperationRecord>& getRedoStack() const {
        return redoStack;
    }

    /**
     * Serialize all operations to JSON
     * @return JSON array of all operations
     */
    nlohmann::json serializeHistory() const {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& op : executedOperations) {
            j.push_back(op->serialize());
        }
        return j;
    }
};
