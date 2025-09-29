#pragma once

#include "Renderer.h"
#include "../core/DynamicArray.h"
#include <string>

class LinearRenderer : public Renderer {
private:
    static constexpr float ELEMENT_WIDTH = 60.0f;
    static constexpr float ELEMENT_HEIGHT = 40.0f;
    static constexpr float ELEMENT_SPACING = 10.0f;
    static constexpr float INDEX_LABEL_HEIGHT = 20.0f;

public:
    void render(const DataStructure* dataStructure, const ImVec2& position, const ImVec2& size) override {
        if (!canRender(dataStructure->getType())) {
            return;
        }
        
        const DynamicArray* array = static_cast<const DynamicArray*>(dataStructure);
        const auto& elements = array->getElements();
        
        if (elements.empty()) {
            // Draw empty array indicator
            ImVec2 emptyPos = ImVec2(position.x + 10, position.y + INDEX_LABEL_HEIGHT + 10);
            drawBox(emptyPos, ImVec2(ELEMENT_WIDTH * 2, ELEMENT_HEIGHT), 
                   IM_COL32(60, 60, 60, 255), IM_COL32(100, 100, 100, 255));
            drawText(ImVec2(emptyPos.x + 15, emptyPos.y + 12), "Empty Array", IM_COL32_WHITE);
            return;
        }
        
        // Calculate starting position to center the array
        float totalWidth = elements.size() * (ELEMENT_WIDTH + ELEMENT_SPACING) - ELEMENT_SPACING;
        float startX = position.x + (size.x - totalWidth) * 0.5f;
        float elementY = position.y + INDEX_LABEL_HEIGHT + 10;
        
        // Draw elements
        for (size_t i = 0; i < elements.size(); ++i) {
            float elementX = startX + i * (ELEMENT_WIDTH + ELEMENT_SPACING);
            
            // Draw index label above element
            std::string indexLabel = std::to_string(i);
            ImVec2 indexPos = ImVec2(elementX + ELEMENT_WIDTH * 0.5f - 5, position.y + 5);
            drawText(indexPos, indexLabel, IM_COL32(200, 200, 200, 255));
            
            // Draw element box
            ImVec2 elementPos = ImVec2(elementX, elementY);
            ImU32 elementColor = IM_COL32(70, 130, 180, 255);  // Steel blue
            ImU32 borderColor = IM_COL32(100, 149, 237, 255);  // Cornflower blue
            
            drawBox(elementPos, ImVec2(ELEMENT_WIDTH, ELEMENT_HEIGHT), elementColor, borderColor);
            
            // Draw element value
            std::string valueText = std::to_string(elements[i]);
            ImVec2 textSize = ImGui::CalcTextSize(valueText.c_str());
            ImVec2 textPos = ImVec2(elementPos.x + (ELEMENT_WIDTH - textSize.x) * 0.5f,
                                   elementPos.y + (ELEMENT_HEIGHT - textSize.y) * 0.5f);
            drawText(textPos, valueText, IM_COL32_WHITE);
            
            // Draw arrows between elements (except for last element)
            if (i < elements.size() - 1) {
                ImVec2 arrowStart = ImVec2(elementX + ELEMENT_WIDTH, elementY + ELEMENT_HEIGHT * 0.5f);
                ImVec2 arrowEnd = ImVec2(elementX + ELEMENT_WIDTH + ELEMENT_SPACING, elementY + ELEMENT_HEIGHT * 0.5f);
                drawArrow(arrowStart, arrowEnd, IM_COL32(150, 150, 150, 255));
            }
        }
        
        // Draw array info
        std::string info = "Size: " + std::to_string(elements.size());
        ImVec2 infoPos = ImVec2(position.x + 10, position.y + INDEX_LABEL_HEIGHT + ELEMENT_HEIGHT + 30);
        drawText(infoPos, info, IM_COL32(200, 200, 200, 255));
    }
    
    void update(float deltaTime) override {
        // For Phase 1, no animations yet
        (void)deltaTime;
    }
    
    ImVec2 getRequiredSize(const DataStructure* dataStructure) const override {
        if (!canRender(dataStructure->getType())) {
            return ImVec2(200, 100);
        }
        
        const DynamicArray* array = static_cast<const DynamicArray*>(dataStructure);
        const auto& elements = array->getElements();
        
        if (elements.empty()) {
            return ImVec2(ELEMENT_WIDTH * 2 + 20, INDEX_LABEL_HEIGHT + ELEMENT_HEIGHT + 60);
        }
        
        float width = elements.size() * (ELEMENT_WIDTH + ELEMENT_SPACING) - ELEMENT_SPACING + 20;
        float height = INDEX_LABEL_HEIGHT + ELEMENT_HEIGHT + 60;
        
        return ImVec2(width, height);
    }
    
    bool canRender(const std::string& dataStructureType) const override {
        return dataStructureType == "DynamicArray";
    }
};