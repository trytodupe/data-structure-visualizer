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
private:
    // Pair of operation and the data structure it was executed on
    struct OperationRecord {
        std::unique_ptr<UserOperation> operation;
        DataStructure* dataStructure;
    };

    std::vector<std::unique_ptr<UserOperation>> executedOperations;
    std::stack<OperationRecord> undoStack;
    std::stack<OperationRecord> redoStack;

public:
    OperationManager() {}

    /**
     * Execute a user operation and add it to history
     * @param ds The data structure to execute on
     * @param userOp The user operation to execute
     */
    void executeOperation(DataStructure& ds, std::unique_ptr<UserOperation> userOp) {
        if (!userOp) return;

        // Execute the user operation
        userOp->execute(ds);

        // Clear redo stack when a new operation is executed
        while (!redoStack.empty()) {
            redoStack.pop();
        }

        // Add to undo stack (clone the user operation and store data structure pointer)
        OperationRecord record;
        record.operation = userOp->clone();
        record.dataStructure = &ds;
        undoStack.push(std::move(record));

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

        auto record = std::move(undoStack.top());
        undoStack.pop();

        // Use the stored data structure pointer
        record.operation->undo(*record.dataStructure);

        redoStack.push(std::move(record));

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

        auto record = std::move(redoStack.top());
        redoStack.pop();

        // Use the stored data structure pointer
        record.operation->execute(*record.dataStructure);

        undoStack.push(std::move(record));

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
        while (!undoStack.empty()) undoStack.pop();
        while (!redoStack.empty()) redoStack.pop();
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
     * Iterate through undo stack and call function for each operation
     * @param func Function to call with (index, operation) - index 0 is most recent
     */
    template<typename Func>
    void forEachUndoOperation(Func func) const {
        // Access the underlying deque container of std::stack
        // This is implementation-dependent but works with libstdc++
        struct StackAccessor : public std::stack<OperationRecord> {
            static const auto& getContainer(const std::stack<OperationRecord>& s) {
                return s.*(&StackAccessor::c);
            }
        };

        const auto& container = StackAccessor::getContainer(undoStack);

        // Iterate from end to beginning (most recent first)
        size_t index = 0;
        for (auto it = container.rbegin(); it != container.rend(); ++it, ++index) {
            func(index, it->operation.get());
        }
    }

    /**
     * Iterate through redo stack and call function for each operation
     * @param func Function to call with (index, operation) - index 0 is most recent
     */
    template<typename Func>
    void forEachRedoOperation(Func func) const {
        // Access the underlying deque container of std::stack
        struct StackAccessor : public std::stack<OperationRecord> {
            static const auto& getContainer(const std::stack<OperationRecord>& s) {
                return s.*(&StackAccessor::c);
            }
        };

        const auto& container = StackAccessor::getContainer(redoStack);

        // Iterate from end to beginning (most recent first)
        size_t index = 0;
        for (auto it = container.rbegin(); it != container.rend(); ++it, ++index) {
            func(index, it->operation.get());
        }
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
