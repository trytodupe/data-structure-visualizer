#pragma once
#include "imgui.h"
#include <string>

/**
 * GUI-based visualizer using Dear ImGui
 */
class GuiVisualizer {
private:
    float width;
    float height;
    ImDrawList* drawList;

public:
    GuiVisualizer(float w = 800.0f, float h = 600.0f)
        : width(w), height(h), drawList(nullptr) {}

    void beginFrame() {
        ImGui::Begin("Data Structure Visualizer", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(width, height));
        drawList = ImGui::GetWindowDrawList();
    }

    void endFrame() {
        ImGui::End();
    }

    void clear() {
        // ImGui clears automatically each frame
    }

    void drawArrayElement(float x, float y, float w, float h,
                         int value, bool highlighted = false) {
        if (!drawList) return;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 p_min(pos.x + x, pos.y + y);
        ImVec2 p_max(pos.x + x + w, pos.y + y + h);

        // Draw rectangle
        ImU32 col = highlighted ? IM_COL32(255, 100, 100, 255) : IM_COL32(100, 100, 255, 255);
        ImU32 border = IM_COL32(255, 255, 255, 255);
        drawList->AddRectFilled(p_min, p_max, col);
        drawList->AddRect(p_min, p_max, border, 0.0f, 0, 2.0f);

        // Draw value
        std::string text = std::to_string(value);
        ImVec2 text_pos(pos.x + x + w / 2 - 10, pos.y + y + h / 2 - 10);
        drawList->AddText(text_pos, IM_COL32(255, 255, 255, 255), text.c_str());
    }

    void drawStackElement(float x, float y, float w, float h,
                         int value, bool isTop = false) {
        if (!drawList) return;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 p_min(pos.x + x, pos.y + y);
        ImVec2 p_max(pos.x + x + w, pos.y + y + h);

        // Draw rectangle
        ImU32 col = isTop ? IM_COL32(255, 200, 100, 255) : IM_COL32(150, 150, 200, 255);
        ImU32 border = IM_COL32(255, 255, 255, 255);
        drawList->AddRectFilled(p_min, p_max, col);
        drawList->AddRect(p_min, p_max, border, 0.0f, 0, 2.0f);

        // Draw value
        std::string text = std::to_string(value);
        ImVec2 text_pos(pos.x + x + w / 2 - 10, pos.y + y + h / 2 - 10);
        drawList->AddText(text_pos, IM_COL32(255, 255, 255, 255), text.c_str());

        // Draw "TOP" indicator
        if (isTop) {
            ImVec2 top_pos(pos.x + x + w + 10, pos.y + y + h / 2 - 10);
            drawList->AddText(top_pos, IM_COL32(255, 200, 100, 255), "TOP");
        }
    }

    void drawTreeNode(float x, float y, float radius,
                     int value, bool highlighted = false) {
        if (!drawList) return;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 center(pos.x + x, pos.y + y);

        // Draw circle
        ImU32 col = highlighted ? IM_COL32(255, 100, 100, 255) : IM_COL32(100, 200, 100, 255);
        ImU32 border = IM_COL32(255, 255, 255, 255);
        drawList->AddCircleFilled(center, radius, col);
        drawList->AddCircle(center, radius, border, 0, 2.0f);

        // Draw value
        std::string text = std::to_string(value);
        ImVec2 text_pos(center.x - 10, center.y - 10);
        drawList->AddText(text_pos, IM_COL32(255, 255, 255, 255), text.c_str());
    }

    void drawEdge(float x1, float y1, float x2, float y2,
                 bool highlighted = false) {
        if (!drawList) return;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 p1(pos.x + x1, pos.y + y1);
        ImVec2 p2(pos.x + x2, pos.y + y2);

        ImU32 col = highlighted ? IM_COL32(255, 200, 100, 255) : IM_COL32(200, 200, 200, 255);
        drawList->AddLine(p1, p2, col, highlighted ? 3.0f : 2.0f);
    }

    void drawText(float x, float y, const std::string& text) {
        if (!drawList) return;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 text_pos(pos.x + x, pos.y + y);
        drawList->AddText(text_pos, IM_COL32(255, 255, 255, 255), text.c_str());
    }

    void drawLabel(float x, float y, const std::string& label) {
        if (!drawList) return;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 text_pos(pos.x + x, pos.y + y);
        drawList->AddText(text_pos, IM_COL32(255, 255, 100, 255), label.c_str());
    }

    float getWidth() const {
        return width;
    }

    float getHeight() const {
        return height;
    }

    void setSize(float w, float h) {
        width = w;
        height = h;
    }
};
