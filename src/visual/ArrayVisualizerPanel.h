#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "../data_structure/ArrayStructure.h"
#include "../operation/ArrayOps.h"
#include <set>
#include <memory>

/**
 * Panel for array visualization and operation controls.
 * Handles rendering array visual representation, operation selection,
 * and user inputs for array operations.
 */
class ArrayVisualizerPanel {
private:
    // Input state
    int insertIndex;
    int insertValue;
    int deleteIndex;
    int selectedOperation;

public:
    ArrayVisualizerPanel()
        : insertIndex(0), insertValue(99),
          deleteIndex(0), selectedOperation(0) {}

    /**
     * Render the complete array panel (visualization + controls)
     */
    void render(ArrayStructure& arrayDS, VisualizationController& controller) {
        ImGui::Text("Array Operations:");
        ImGui::Spacing();

        // Display current array state visually
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Array:");
        ImGui::Text("Size: %zu / %zu", arrayDS.size(), arrayDS.capacity());
        ImGui::Spacing();

        // Get modified indices for highlighting
        std::set<size_t> modifiedIndices = getModifiedIndices(controller);

        // Render visual representation
        renderVisual(arrayDS, controller, modifiedIndices);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Render operation controls
        renderOperationControls(arrayDS, controller);
    }

private:
    /**
     * Get indices being modified in current atomic operation
     */
    std::set<size_t> getModifiedIndices(VisualizationController& controller) {
        std::set<size_t> modifiedIndices;

        if (!controller.isInVisualizationPhase()) {
            return modifiedIndices;
        }

        Operation* currentOp = controller.getCurrentAtomicOperation();
        if (!currentOp) {
            return modifiedIndices;
        }

        // Try to cast to WriteOp to get the modified index
        if (auto* writeOp = dynamic_cast<WriteOp*>(currentOp)) {
            std::string desc = writeOp->getDescription();
            size_t indexPos = desc.find("index ");
            if (indexPos != std::string::npos) {
                size_t idx = std::stoul(desc.substr(indexPos + 6));
                modifiedIndices.insert(idx);
            }
        }
        // Try to cast to MoveOp to get both indices
        else if (auto* moveOp = dynamic_cast<MoveOp*>(currentOp)) {
            std::string desc = moveOp->getDescription();
            size_t fromPos = desc.find("from ");
            size_t toPos = desc.find("to ");
            if (fromPos != std::string::npos && toPos != std::string::npos) {
                size_t fromIdx = std::stoul(desc.substr(fromPos + 5));
                size_t toIdx = std::stoul(desc.substr(toPos + 3));
                modifiedIndices.insert(fromIdx);
                modifiedIndices.insert(toIdx);
            }
        }

        return modifiedIndices;
    }

    /**
     * Render the visual representation of the array
     */
    void renderVisual(ArrayStructure &arrayDS, VisualizationController &controller,
                      const std::set<size_t> &modifiedIndices)
    {
        const float box_size = 50.0f;
        const float spacing = 10.0f;
        ImVec2 start_pos = ImGui::GetCursorScreenPos();

        // 1. Draw the data structure base state
        ImVec2 drawnSize = arrayDS.draw(start_pos, box_size, spacing);

        // 2. Draw operation overlay if visualizing
        if (controller.isInVisualizationPhase())
        {
            const auto *currentOp = controller.getCurrentAtomicOperation();
            if (currentOp)
            {
                currentOp->drawOverlay(arrayDS, start_pos, box_size, spacing);
            }
        }

        // 3. Reserve space for the drawn area
        ImGui::Dummy(drawnSize);
    }

    /**
     * Render operation selection and controls
     */
    void renderOperationControls(ArrayStructure& arrayDS, VisualizationController& controller) {
        ImGui::Text("Select Array Operation:");
        const char* arrayOps[] = { "Insert", "Delete", "Reset" };
        ImGui::Combo("##arrayOperation", &selectedOperation, arrayOps, IM_ARRAYSIZE(arrayOps));
        ImGui::Spacing();

        if (selectedOperation == 0) { // Insert
            ImGui::Text("Insert Operation:");
            ImGui::InputInt("Index##insert", &insertIndex);
            ImGui::InputInt("Value##insert", &insertValue);
            if (ImGui::Button("Execute Insert")) {
                if (insertIndex >= 0 && insertIndex <= (int)arrayDS.size()) {
                    controller.stageOperation(std::make_unique<ArrayInsert>(arrayDS, insertIndex, insertValue), &arrayDS);
                }
            }
        } else if (selectedOperation == 1) { // Delete
            ImGui::Text("Delete Operation:");
            ImGui::InputInt("Index##delete", &deleteIndex);
            if (ImGui::Button("Execute Delete")) {
                if (deleteIndex >= 0 && deleteIndex < (int)arrayDS.size()) {
                    controller.stageOperation(std::make_unique<ArrayDelete>(arrayDS, deleteIndex), &arrayDS);
                }
            }
        } else if (selectedOperation == 2) { // Reset
            ImGui::Text("Reset Operation:");
            ImGui::Text("Resets array to [5, 2, 8, 1, 9].");
            if (ImGui::Button("Execute Reset")) {
                std::vector<int> resetValues = {5, 2, 8, 1, 9};
                controller.stageOperation(std::make_unique<ArrayInit>(resetValues), &arrayDS);
            }
        }
    }
};
