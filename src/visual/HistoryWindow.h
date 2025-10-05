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

        ImGui::Text("Operation Stacks:");
        ImGui::Separator();
        ImGui::Spacing();

        // Display stack sizes
        size_t undoSize = opManager.getUndoStackSize();
        size_t redoSize = opManager.getRedoStackSize();
        ImGui::Text("Undo stack: %zu | Redo stack: %zu", undoSize, redoSize);

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

        // Scrollable history list showing both stacks
        ImGui::BeginChild("HistoryList", ImVec2(0, 0), true);

        // Display Undo Stack (green color)
        if (undoSize > 0) {
            ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1.0f), "=== Undo Stack (most recent on top) ===");
            const auto& undoStack = opManager.getUndoStack();
            // Iterate from end to beginning (most recent first)
            for (int i = undoStack.size() - 1; i >= 0; --i) {
                const auto& record = undoStack[i];
                ImGui::PushID(i);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
                ImGui::Text("  [%d] %s - %s",
                           i + 1,
                           record.operation->getName().c_str(),
                           record.operation->getDescription().c_str());
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
            ImGui::Spacing();
        }

        // Display Redo Stack (yellow/orange color)
        if (redoSize > 0) {
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), "=== Redo Stack (most recent on top) ===");
            const auto& redoStack = opManager.getRedoStack();
            // Iterate from end to beginning (most recent first)
            for (int i = redoStack.size() - 1; i >= 0; --i) {
                const auto& record = redoStack[i];
                ImGui::PushID(i + 1000); // Offset ID to avoid collision
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.5f, 1.0f));
                ImGui::Text("  [%d] %s - %s",
                           i + 1,
                           record.operation->getName().c_str(),
                           record.operation->getDescription().c_str());
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }

        if (undoSize == 0 && redoSize == 0) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No operations yet...");
        }

        ImGui::EndChild();

        ImGui::End();
    }
};
