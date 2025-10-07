#pragma once
#include "../core/Operation.h"
#include "../core/UserOperation.h"
#include "../data_structure/StackStructure.h"
#include "../visual/GuiVisualizer.h"
#include <memory>

// ============================================================================
// ATOMIC OPERATIONS
// ============================================================================

/**
 * Atomic operation: Push value onto stack
 */
class PushOp : public Operation {
private:
    int value;
    bool wasSuccessful;

public:
    PushOp(int val) : value(val), wasSuccessful(false) {}

    void execute(DataStructure& ds) override {
        StackStructure& stack = dynamic_cast<StackStructure&>(ds);
        stack.data.push(value);
        wasSuccessful = true;
    }

    void undo(DataStructure& ds) override {
        if (wasSuccessful) {
            StackStructure& stack = dynamic_cast<StackStructure&>(ds);
            if (!stack.data.empty()) {
                stack.data.pop();
            }
        }
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Highlight the top of the stack (where we're pushing)
        float x = startPos.x;
        float y = startPos.y;

        // Draw orange highlight on top box
        ImVec2 topLeft(x, y);
        ImVec2 bottomRight(x + boxSize, y + boxSize);
        drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 140, 0, 255), 0.0f, 0, 3.0f);
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "PushOp";
        j["value"] = value;
        j["wasSuccessful"] = wasSuccessful;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        value = j["value"];
        wasSuccessful = j["wasSuccessful"];
    }

    std::string getDescription() const override {
        return "Push " + std::to_string(value) + " onto stack";
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<PushOp>(value);
        op->wasSuccessful = wasSuccessful;
        return op;
    }
};

/**
 * Atomic operation: Pop value from stack
 */
class PopOp : public Operation {
private:
    int poppedValue;
    bool wasEmpty;

public:
    PopOp() : poppedValue(0), wasEmpty(true) {}

    void execute(DataStructure& ds) override {
        StackStructure& stack = dynamic_cast<StackStructure&>(ds);
        wasEmpty = stack.data.empty();
        if (!wasEmpty) {
            poppedValue = stack.data.top();
            stack.data.pop();
        }
    }

    void undo(DataStructure& ds) override {
        if (!wasEmpty) {
            StackStructure& stack = dynamic_cast<StackStructure&>(ds);
            stack.data.push(poppedValue);
        }
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        if (!wasEmpty) {
            // Highlight the top of the stack (where we're popping from)
            float x = startPos.x;
            float y = startPos.y;

            // Draw orange highlight on top box
            ImVec2 topLeft(x, y);
            ImVec2 bottomRight(x + boxSize, y + boxSize);
            drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 140, 0, 255), 0.0f, 0, 3.0f);
        }
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "PopOp";
        j["poppedValue"] = poppedValue;
        j["wasEmpty"] = wasEmpty;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        poppedValue = j["poppedValue"];
        wasEmpty = j["wasEmpty"];
    }

    std::string getDescription() const override {
        if (wasEmpty) {
            return "Pop from empty stack";
        }
        return "Pop " + std::to_string(poppedValue) + " from stack";
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<PopOp>();
        op->poppedValue = poppedValue;
        op->wasEmpty = wasEmpty;
        return op;
    }
};

// ============================================================================
// USER OPERATIONS (Composite)
// ============================================================================

/**
 * User operation: Initialize stack with values
 */
class StackInit : public UserOperation {
public:
    StackInit(StackStructure& stack, const std::vector<int>& values)
        : UserOperation("StackInit", "Initialize stack with values") {
        // Clear the stack directly (not using atomic operations)
        while (!stack.data.empty()) {
            stack.data.pop();
        }

        // Push new values
        for (int value : values) {
            operations.push_back(std::make_unique<PushOp>(value));
        }
    }
};

/**
 * User operation: Push value onto stack (wrapper for atomic op)
 */
class StackPush : public UserOperation {
public:
    StackPush(int value)
        : UserOperation("StackPush", "Push value onto stack") {
        operations.push_back(std::make_unique<PushOp>(value));
    }
};

/**
 * User operation: Pop value from stack (wrapper for atomic op)
 */
class StackPop : public UserOperation {
public:
    StackPop()
        : UserOperation("StackPop", "Pop value from stack") {
        operations.push_back(std::make_unique<PopOp>());
    }
};

/**
 * User operation: Clear entire stack
 */
class StackClear : public UserOperation {
public:
    StackClear(StackStructure& stack)
        : UserOperation("StackClear", "Clear all elements from stack") {
        // Pop all elements
        size_t size = stack.data.size();
        for (size_t i = 0; i < size; ++i) {
            operations.push_back(std::make_unique<PopOp>());
        }
    }
};

/**
 * User operation: Reverse stack
 */
class StackReverse : public UserOperation {
public:
    StackReverse(StackStructure& stack)
        : UserOperation("StackReverse", "Reverse stack order") {
        // Get all elements
        std::vector<int> elements;
        std::stack<int> temp = stack.data;
        while (!temp.empty()) {
            elements.push_back(temp.top());
            temp.pop();
        }

        // Pop all
        for (size_t i = 0; i < elements.size(); ++i) {
            operations.push_back(std::make_unique<PopOp>());
        }

        // Push in same order (which reverses)
        for (int value : elements) {
            operations.push_back(std::make_unique<PushOp>(value));
        }
    }
};
