#pragma once
#include "imgui.h"
#include "../core/OperationManager.h"

/**
 * Displays operation history in a separate ImGui window
 */
class HistoryWindow {
private:
    bool isOpen;
    float windowScale;

public:
    HistoryWindow(float scale = 1.0f)
        : isOpen(true), windowScale(scale) {}

    /**
     * Show/hide the history window
     */
    void setOpen(bool open) {
        isOpen = open;
    }

    bool getOpen() const {
        return isOpen;
    }

    /**
     * Render the history window
     * @param opManager The operation manager containing the history
     */
    void render(OperationManager& opManager) {
        if (!isOpen) return;

        ImGui::SetNextWindowSize(ImVec2(400 * windowScale, 300 * windowScale), ImGuiCond_FirstUseEver);
        ImGui::Begin("Operation History", &isOpen);

        ImGui::Text("All executed operations:");
        ImGui::Separator();
        ImGui::Spacing();

        // Display operation count
        const auto& history = opManager.getExecutedOperations();
        ImGui::Text("Total operations: %zu", history.size());

        ImGui::Spacing();

        // Undo/Redo controls
        if (ImGui::Button("Undo")) {
            opManager.undo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Redo")) {
            opManager.redo();
        }
        ImGui::SameLine();
        ImGui::Text("| Can undo: %s | Can redo: %s",
                   opManager.canUndo() ? "Yes" : "No",
                   opManager.canRedo() ? "Yes" : "No");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Scrollable history list
        ImGui::BeginChild("HistoryList", ImVec2(0, 0), true);
        for (size_t i = 0; i < history.size(); ++i) {
            ImGui::PushID(i);

            // Highlight with alternating colors
            if (i % 2 == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            }

            ImGui::Text("%zu: %s - %s",
                       i + 1,
                       history[i]->getName().c_str(),
                       history[i]->getDescription().c_str());

            ImGui::PopStyleColor();
            ImGui::PopID();
        }

        if (history.empty()) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No operations yet...");
        }

        ImGui::EndChild();

        ImGui::End();
    }
};
