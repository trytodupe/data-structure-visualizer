#pragma once
#include "../core/Operation.h"
#include "../core/UserOperation.h"
#include "../data_structure/ArrayStructure.h"
#include "../visual/GuiVisualizer.h"
#include <memory>
#include <iostream>

// ============================================================================
// ATOMIC OPERATIONS
// ============================================================================

/**
 * Atomic operation: Write a value to an array index
 */
class WriteOp : public Operation {
private:
    size_t index;
    int oldValue;
    int newValue;

public:
    WriteOp(size_t idx, int newV)
        : index(idx), oldValue(0), newValue(newV) {}

    WriteOp(size_t idx, int oldV, int newV)
        : index(idx), oldValue(oldV), newValue(newV) {}

    void execute(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        // Capture old value before modification
        oldValue = (index < arr.size()) ? arr[index] : 0;

        if (index >= arr.size()) {
            arr.resize(index + 1);
        }
        arr[index] = newValue;
    }

    void undo(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        if (index == arr.size() - 1) {
            arr.resize(index);
        }
        arr[index] = oldValue;
    }

    void draw(GuiVisualizer& vis) override {
        // Visual representation of write operation
        // This will be implemented by the specific visualizer
        float x = 50.0f + index * 60.0f;
        float y = 100.0f;
        vis.drawArrayElement(x, y, 50.0f, 50.0f, newValue, true);
        vis.drawLabel(x, y - 20.0f, "Write");
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "WriteOp";
        j["index"] = index;
        j["oldValue"] = oldValue;
        j["newValue"] = newValue;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        index = j["index"];
        oldValue = j["oldValue"];
        newValue = j["newValue"];
    }

    std::string getDescription() const override {
        return "Write " + std::to_string(newValue) + " to index " + std::to_string(index);
    }

    std::unique_ptr<Operation> clone() const override {
        return std::make_unique<WriteOp>(index, oldValue, newValue);
    }
};

/**
 * Atomic operation: Move element from one index to another (copy operation)
 */
class MoveOp : public Operation {
private:
    size_t fromIndex;
    size_t toIndex;
    int oldValue;  // Old value at toIndex

public:
    MoveOp(size_t from, size_t to)
        : fromIndex(from), toIndex(to), oldValue(0) {}

    void execute(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        if (fromIndex < arr.size() && toIndex < arr.size()) {
            // Capture old value at destination
            oldValue = arr[toIndex];
            // Copy from source to destination
            arr[toIndex] = arr[fromIndex];
        }
    }

    void undo(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        // Restore old value at destination
        if (toIndex < arr.size()) {
            arr[toIndex] = oldValue;
        }
    }

    void draw(GuiVisualizer& vis) override {
        float x1 = 50.0f + fromIndex * 60.0f;
        float x2 = 50.0f + toIndex * 60.0f;
        float y = 100.0f;
        vis.drawArrayElement(x1, y, 50.0f, 50.0f, 0, true);
        vis.drawArrayElement(x2, y, 50.0f, 50.0f, 0, true);
        vis.drawEdge(x1 + 25.0f, y + 25.0f, x2 + 25.0f, y + 25.0f, true);
        vis.drawLabel(x1, y - 20.0f, "Swap");
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "MoveOp";
        j["fromIndex"] = fromIndex;
        j["toIndex"] = toIndex;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        fromIndex = j["fromIndex"];
        toIndex = j["toIndex"];
    }

    std::string getDescription() const override {
        return "Move element from " + std::to_string(fromIndex) + " to " + std::to_string(toIndex);
    }

    std::unique_ptr<Operation> clone() const override {
        return std::make_unique<MoveOp>(fromIndex, toIndex);
    }
};

// ============================================================================
// USER OPERATIONS (Composite)
// ============================================================================

/**
 * User operation: Initialize array with values
 */
class ArrayInit : public UserOperation {
public:
    ArrayInit(const std::vector<int>& values)
        : UserOperation("ArrayInit", "Initialize array with values") {
        for (size_t i = 0; i < values.size(); ++i) {
            operations.push_back(std::make_unique<WriteOp>(i, values[i]));
        }
    }
};

/**
 * User operation: Insert element at position
 */
class ArrayInsert : public UserOperation {
public:
    ArrayInsert(ArrayStructure& arr, size_t index, int value)
        : UserOperation("ArrayInsert", "Insert element into array") {
        // Check if we can expand the array
        if (arr.size() >= arr.capacity()) {
            return; // Cannot insert, array is full
        }

        // Shift elements right by moving from the end backwards
        // Start from the last element and move each one position to the right
        for (int i = (int)arr.size() - 1; i >= (int)index; --i) {
            // Move element at i to position i+1
            operations.push_back(std::make_unique<MoveOp>(i, i + 1));
        }

        // Finally, write the new value at the insertion point
        operations.push_back(std::make_unique<WriteOp>(index, value));
    }
};

/**
 * User operation: Delete element at position
 */
class ArrayDelete : public UserOperation {
public:
    ArrayDelete(ArrayStructure& arr, size_t index)
        : UserOperation("ArrayDelete", "Delete element from array") {
        // Shift elements left - operations will capture old values during execute
        for (size_t i = index; i < arr.size() - 1; ++i) {
            int val = arr[i + 1];
            operations.push_back(
                std::make_unique<WriteOp>(i, val)
            );
        }
        // Clear last element
        if (arr.size() > 0) {
            operations.push_back(
                std::make_unique<WriteOp>(arr.size() - 1, 0)
            );
        }
    }
};


