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
 * Atomic operation: Resize array
 */
class ResizeOp : public Operation
{
private:
    size_t newSize;
    size_t oldSize;

public:
    ResizeOp(size_t size) : newSize(size), oldSize(0) {}

    void execute(DataStructure &ds) override
    {
        ArrayStructure &arr = dynamic_cast<ArrayStructure &>(ds);
        oldSize = arr.size();
        arr.resize(newSize);
    }

    void undo(DataStructure &ds) override
    {
        ArrayStructure &arr = dynamic_cast<ArrayStructure &>(ds);
        arr.resize(oldSize);
    }

    void drawOverlay(const DataStructure &ds, ImVec2 startPos, float boxSize, float spacing) const override
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Draw a big rectangle showing the new array size
        float totalWidth = newSize * boxSize + (newSize - 1) * spacing;
        float x = startPos.x;
        float y = startPos.y;

        // Draw orange rectangle encompassing the new size
        ImVec2 topLeft(x - 5.0f, y - 5.0f);
        ImVec2 bottomRight(x + totalWidth + 5.0f, y + boxSize + 5.0f);
        drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 140, 0, 255), 0.0f, 0, 4.0f);

        // Draw text showing the resize operation
        std::string resizeText = "Resize to " + std::to_string(newSize);
        ImVec2 textSize = ImGui::CalcTextSize(resizeText.c_str());
        ImVec2 textPos(x + (totalWidth - textSize.x) * 0.5f, y + boxSize + 15.0f);
        drawList->AddText(textPos, IM_COL32(255, 140, 0, 255), resizeText.c_str());
    }

    nlohmann::json serialize() const override
    {
        nlohmann::json j;
        j["type"] = "ResizeOp";
        j["newSize"] = newSize;
        j["oldSize"] = oldSize;
        return j;
    }

    void deserialize(const nlohmann::json &j) override
    {
        newSize = j["newSize"];
        oldSize = j["oldSize"];
    }

    std::string getDescription() const override
    {
        return "Resize array to " + std::to_string(newSize);
    }

    std::unique_ptr<Operation> clone() const override
    {
        auto op = std::make_unique<ResizeOp>(newSize);
        op->oldSize = oldSize;
        return op;
    }
};

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
        oldValue = arr[index];
        arr[index] = newValue;
    }

    void undo(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        arr[index] = oldValue;
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Highlight the index being written to
        float x = startPos.x + index * (boxSize + spacing);
        float y = startPos.y;

        // Draw orange highlight
        ImVec2 topLeft(x, y);
        ImVec2 bottomRight(x + boxSize, y + boxSize);
        drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 140, 0, 200));
        drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 0, 255), 0.0f, 0, 3.0f);

        // Draw new value
        char valueText[16];
        snprintf(valueText, sizeof(valueText), "%d", newValue);
        ImVec2 textSize = ImGui::CalcTextSize(valueText);
        ImVec2 textPos(x + (boxSize - textSize.x) * 0.5f, y + (boxSize - textSize.y) * 0.5f);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), valueText);
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
        oldValue = arr[toIndex];
        arr[toIndex] = arr[fromIndex];
    }

    void undo(DataStructure& ds) override {
        ArrayStructure& arr = dynamic_cast<ArrayStructure&>(ds);
        arr[fromIndex] = arr[toIndex];
        arr[toIndex] = oldValue;
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Highlight both indices
        float x1 = startPos.x + fromIndex * (boxSize + spacing);
        float x2 = startPos.x + toIndex * (boxSize + spacing);
        float y = startPos.y;

        // Draw orange highlight on both boxes
        ImVec2 topLeft1(x1, y);
        ImVec2 bottomRight1(x1 + boxSize, y + boxSize);
        drawList->AddRect(topLeft1, bottomRight1, IM_COL32(255, 140, 0, 255), 0.0f, 0, 3.0f);

        ImVec2 topLeft2(x2, y);
        ImVec2 bottomRight2(x2 + boxSize, y + boxSize);
        drawList->AddRect(topLeft2, bottomRight2, IM_COL32(255, 140, 0, 255), 0.0f, 0, 3.0f);

        // Draw arrow from source to destination
        ImVec2 arrowStart(x1 + boxSize * 0.5f, y - 10.0f);
        ImVec2 arrowEnd(x2 + boxSize * 0.5f, y - 10.0f);
        drawList->AddLine(arrowStart, arrowEnd, IM_COL32(255, 140, 0, 255), 2.0f);

        // Draw arrowhead
        float arrowSize = 8.0f;
        ImVec2 arrowTip = arrowEnd;
        ImVec2 arrowLeft(arrowEnd.x - arrowSize, arrowEnd.y - arrowSize);
        ImVec2 arrowRight(arrowEnd.x - arrowSize, arrowEnd.y + arrowSize);
        if (fromIndex < toIndex) {
            drawList->AddTriangleFilled(arrowTip, arrowLeft, arrowRight, IM_COL32(255, 140, 0, 255));
        } else {
            ImVec2 arrowTip2(arrowEnd.x, arrowEnd.y);
            ImVec2 arrowLeft2(arrowEnd.x + arrowSize, arrowEnd.y - arrowSize);
            ImVec2 arrowRight2(arrowEnd.x + arrowSize, arrowEnd.y + arrowSize);
            drawList->AddTriangleFilled(arrowTip2, arrowLeft2, arrowRight2, IM_COL32(255, 140, 0, 255));
        }
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
        // First resize the array to accommodate all values
        if (!values.empty()) {
            operations.push_back(std::make_unique<ResizeOp>(values.size()));
            // Then write each value
            for (size_t i = 0; i < values.size(); ++i) {
                operations.push_back(std::make_unique<WriteOp>(i, values[i]));
            }
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

        size_t oldSize = arr.size();

        // First, expand the array using ResizeOp (not WriteOp!)
        operations.push_back(std::make_unique<ResizeOp>(oldSize + 1));

        // Shift elements right by moving from the end backwards
        // Start from the last element and move each one position to the right
        for (int i = (int)oldSize - 1; i >= (int)index; --i) {
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
        if (arr.size() == 0) {
            return; // Cannot delete from empty array
        }

        // Shift elements left using MoveOp
        for (size_t i = index; i < arr.size() - 1; ++i) {
            // Move element at i+1 to position i
            operations.push_back(std::make_unique<MoveOp>(i + 1, i));
        }

        // Shrink the array by one element
        operations.push_back(std::make_unique<ResizeOp>(arr.size() - 1));
    }
};


