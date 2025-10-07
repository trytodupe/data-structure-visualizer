#pragma once
#include "../core/DataStructure.h"
#include <stack>
#include <vector>
#include <string>
#include <sstream>

/**
 * Stack data structure implementation.
 * Provides LIFO (Last-In-First-Out) operations.
 */
class StackStructure : public DataStructure {
public:
    std::stack<int> data;

    StackStructure() = default;

    void initialize() override {
        while (!data.empty()) {
            data.pop();
        }
    }

    std::string getType() const override {
        return "Stack";
    }

    std::string getStateDescription() const override {
        std::ostringstream oss;
        oss << "Stack[" << data.size() << "]";
        if (!data.empty()) {
            oss << " top=" << data.top();
        } else {
            oss << " (empty)";
        }
        return oss.str();
    }

    ImVec2 draw(ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Convert stack to vector for drawing (top to bottom)
        std::vector<int> elements;
        std::stack<int> tempStack = data;
        while (!tempStack.empty()) {
            elements.push_back(tempStack.top());
            tempStack.pop();
        }

        // Draw from top to bottom
        for (size_t i = 0; i < elements.size(); ++i) {
            float x = startPos.x;
            float y = startPos.y + i * (boxSize + spacing);

            // Draw box
            ImVec2 topLeft(x, y);
            ImVec2 bottomRight(x + boxSize, y + boxSize);
            drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(100, 250, 100, 255));
            drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

            // Draw value
            char valueText[16];
            snprintf(valueText, sizeof(valueText), "%d", elements[i]);
            ImVec2 textSize = ImGui::CalcTextSize(valueText);
            ImVec2 textPos(x + (boxSize - textSize.x) * 0.5f, y + (boxSize - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(0, 0, 0, 255), valueText);

            // Draw "TOP" label for the first element
            if (i == 0) {
                ImVec2 topLabelPos(x + boxSize + 10.0f, y + (boxSize - ImGui::CalcTextSize("TOP").y) * 0.5f);
                drawList->AddText(topLabelPos, IM_COL32(255, 100, 100, 255), "TOP");
            }
        }

        // Return the size of the drawn area
        float width = boxSize + 60.0f; // box + "TOP" label
        float height = elements.size() * (boxSize + spacing);
        return ImVec2(width, height);
    }
};
