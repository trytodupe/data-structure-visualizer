#pragma once
#include "../core/DataStructure.h"
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>

/**
 * Array data structure implementation using C-style array.
 * Provides basic array operations with fixed maximum capacity.
 */
class ArrayStructure : public DataStructure {
private:
    static const size_t MAX_SIZE = 100;
    int data[MAX_SIZE];
    size_t currentSize;

public:
    ArrayStructure() : currentSize(0) {
        std::memset(data, 0, sizeof(data));
    }

    void initialize() override {
        currentSize = 0;
        std::memset(data, 0, sizeof(data));
    }

    std::string getType() const override {
        return "Array";
    }

    std::string getStateDescription() const override {
        std::ostringstream oss;
        oss << "Array[" << currentSize << "/" << MAX_SIZE << "]: [";
        for (size_t i = 0; i < currentSize; ++i) {
            if (i > 0) oss << ", ";
            oss << data[i];
        }
        oss << "]";
        return oss.str();
    }

    ImVec2 draw(ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        for (size_t i = 0; i < currentSize; ++i) {
            float x = startPos.x + i * (boxSize + spacing);
            float y = startPos.y;

            // Draw box
            ImVec2 topLeft(x, y);
            ImVec2 bottomRight(x + boxSize, y + boxSize);
            drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(100, 100, 250, 255));
            drawList->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

            // Draw value
            char valueText[16];
            snprintf(valueText, sizeof(valueText), "%d", data[i]);
            ImVec2 textSize = ImGui::CalcTextSize(valueText);
            ImVec2 textPos(x + (boxSize - textSize.x) * 0.5f, y + (boxSize - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), valueText);

            // Draw index below
            char indexText[16];
            snprintf(indexText, sizeof(indexText), "[%zu]", i);
            ImVec2 indexSize = ImGui::CalcTextSize(indexText);
            ImVec2 indexPos(x + (boxSize - indexSize.x) * 0.5f, y + boxSize + 5.0f);
            drawList->AddText(indexPos, IM_COL32(150, 150, 150, 255), indexText);
        }

        // Return the size of the drawn area
        float width = currentSize * (boxSize + spacing);
        float height = boxSize + 25.0f; // box + index text
        return ImVec2(width, height);
    }

    // Helper methods to maintain interface compatibility
    size_t size() const { return currentSize; }
    size_t capacity() const { return MAX_SIZE; }

    int& operator[](size_t index) { return data[index]; }
    const int& operator[](size_t index) const { return data[index]; }

    void resize(size_t newSize) {
        if (newSize > MAX_SIZE) newSize = MAX_SIZE;
        if (newSize > currentSize) {
            // Zero out new elements
            std::memset(data + currentSize, 0, (newSize - currentSize) * sizeof(int));
        }
        currentSize = newSize;
    }

    int* begin() { return data; }
    const int* begin() const { return data; }
    int* end() { return data + currentSize; }
    const int* end() const { return data + currentSize; }
};
