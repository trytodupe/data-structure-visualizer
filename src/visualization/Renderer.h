#pragma once

#include "../core/DataStructure.h"
#include "imgui.h"

class Renderer {
public:
    virtual ~Renderer() = default;
    
    // Main rendering method
    virtual void render(const DataStructure* dataStructure, const ImVec2& position, const ImVec2& size) = 0;
    
    // Update animation state
    virtual void update(float deltaTime) = 0;
    
    // Get required size for rendering
    virtual ImVec2 getRequiredSize(const DataStructure* dataStructure) const = 0;
    
    // Check if this renderer can handle the given data structure type
    virtual bool canRender(const std::string& dataStructureType) const = 0;
    
protected:
    // Helper methods for common rendering tasks
    void drawBox(const ImVec2& position, const ImVec2& size, ImU32 color, ImU32 borderColor = IM_COL32_WHITE) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y), color);
        drawList->AddRect(position, ImVec2(position.x + size.x, position.y + size.y), borderColor);
    }
    
    void drawText(const ImVec2& position, const std::string& text, ImU32 color = IM_COL32_WHITE) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddText(position, color, text.c_str());
    }
    
    void drawLine(const ImVec2& start, const ImVec2& end, ImU32 color = IM_COL32_WHITE, float thickness = 1.0f) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(start, end, color, thickness);
    }
    
    void drawArrow(const ImVec2& start, const ImVec2& end, ImU32 color = IM_COL32_WHITE, float thickness = 1.5f) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // Draw main line
        drawList->AddLine(start, end, color, thickness);
        
        // Calculate arrow head
        ImVec2 direction = ImVec2(end.x - start.x, end.y - start.y);
        float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
            
            float arrowSize = 8.0f;
            ImVec2 arrowBase = ImVec2(end.x - direction.x * arrowSize, end.y - direction.y * arrowSize);
            ImVec2 perpendicular = ImVec2(-direction.y, direction.x);
            
            ImVec2 arrowPoint1 = ImVec2(arrowBase.x + perpendicular.x * arrowSize * 0.5f, 
                                       arrowBase.y + perpendicular.y * arrowSize * 0.5f);
            ImVec2 arrowPoint2 = ImVec2(arrowBase.x - perpendicular.x * arrowSize * 0.5f, 
                                       arrowBase.y - perpendicular.y * arrowSize * 0.5f);
            
            drawList->AddLine(end, arrowPoint1, color, thickness);
            drawList->AddLine(end, arrowPoint2, color, thickness);
        }
    }
};