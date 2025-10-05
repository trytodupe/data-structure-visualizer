#pragma once
#include "../core/Operation.h"
#include "../core/UserOperation.h"
#include "../data_structure/ArrayStructure.h"
#include "../visual/GuiVisualizer.h"
#include <memory>

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
        oldValue = (index < arr.data.size()) ? arr.data[index] : 0;

        if (index >= arr.data.size()) {
            arr.data.resize(index + 1);
        }
        arr.data[index] = newValue;
    }

    void undo(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        if (index >= arr.data.size()) {
            arr.data.resize(index + 1);
        }
        arr.data[index] = oldValue;
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
 * Atomic operation: Move/swap elements in array
 */
class MoveOp : public Operation {
private:
    size_t fromIndex;
    size_t toIndex;
    int fromValue;
    int toValue;

public:
    MoveOp(size_t from, size_t to)
        : fromIndex(from), toIndex(to), fromValue(0), toValue(0) {}

    void execute(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        if (fromIndex < arr.data.size() && toIndex < arr.data.size()) {
            // Capture values before swap
            fromValue = arr.data[fromIndex];
            toValue = arr.data[toIndex];
            std::swap(arr.data[fromIndex], arr.data[toIndex]);
        }
    }

    void undo(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        // Swap back
        if (fromIndex < arr.data.size() && toIndex < arr.data.size()) {
            std::swap(arr.data[fromIndex], arr.data[toIndex]);
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
        return "Swap elements at " + std::to_string(fromIndex) + " and " + std::to_string(toIndex);
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
        // Shift elements right - operations will capture old values during execute
        for (size_t i = arr.data.size(); i > index; --i) {
            if (i - 1 < arr.data.size()) {
                int val = arr.data[i - 1];
                operations.push_back(
                    std::make_unique<WriteOp>(i, val)
                );
            }
        }
        // Insert new value
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
        for (size_t i = index; i < arr.data.size() - 1; ++i) {
            int val = arr.data[i + 1];
            operations.push_back(
                std::make_unique<WriteOp>(i, val)
            );
        }
        // Clear last element
        if (arr.data.size() > 0) {
            operations.push_back(
                std::make_unique<WriteOp>(arr.data.size() - 1, 0)
            );
        }
    }
};

/**
 * User operation: Sort array (bubble sort for visualization)
 */
class ArraySort : public UserOperation {
public:
    ArraySort(ArrayStructure& arr)
        : UserOperation("ArraySort", "Sort array using bubble sort") {
        // Create a copy to determine swaps needed
        std::vector<int> data = arr.data;
        size_t n = data.size();

        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = 0; j < n - i - 1; ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                    operations.push_back(std::make_unique<MoveOp>(j, j + 1));
                }
            }
        }
    }
};
