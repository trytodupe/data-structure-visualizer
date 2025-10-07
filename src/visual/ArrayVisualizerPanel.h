#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "../data_structure/ArrayStructure.h"
#include "../operation/ArrayOps.h"
#include "../core/OperationManager.h"
#include <set>
#include <memory>
#include <string>
#include <cstring>

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
    char initValuesInput[256];  // Buffer for custom init values

public:
    ArrayVisualizerPanel()
        : insertIndex(0), insertValue(99),
          deleteIndex(0), selectedOperation(0) {
        // Default init values
        strcpy(initValuesInput, "5, 2, 8, 1, 9");
    }

    /**
     * Render the complete array panel (visualization + controls)
     */
    void render(ArrayStructure& arrayDS, VisualizationController& controller, OperationManager& opManager) {
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
        renderOperationControls(arrayDS, controller, opManager);
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
    void renderOperationControls(ArrayStructure& arrayDS, VisualizationController& controller, OperationManager& opManager) {
        ImGui::Text("Select Array Operation:");
        const char* arrayOps[] = { "Init", "Insert", "Delete" };
        ImGui::Combo("##arrayOperation", &selectedOperation, arrayOps, IM_ARRAYSIZE(arrayOps));
        ImGui::Spacing();

        if (selectedOperation == 0) { // Init
            ImGui::Text("Init Operation:");
            ImGui::Text("Initialize array with custom values (comma-separated)");
            ImGui::InputText("Values##init", initValuesInput, IM_ARRAYSIZE(initValuesInput));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: This will clear undo/redo history");
            if (ImGui::Button("Execute Init")) {
                // Parse the input string to extract values
                std::vector<int> values;
                std::string input(initValuesInput);
                std::string current = "";

                for (char c : input) {
                    if (c == ',' || c == ' ') {
                        if (!current.empty()) {
                            try {
                                values.push_back(std::stoi(current));
                                current = "";
                            } catch (...) {
                                // Skip invalid numbers
                            }
                        }
                    } else if ((c >= '0' && c <= '9') || c == '-') {
                        current += c;
                    }
                }
                // Don't forget the last value
                if (!current.empty()) {
                    try {
                        values.push_back(std::stoi(current));
                    } catch (...) {
                        // Skip invalid numbers
                    }
                }

                if (!values.empty()) {
                    // Clear undo/redo history BEFORE staging init operation
                    // This makes Init the starting point with no previous history
                    opManager.clear();
                    controller.stageOperation(std::make_unique<ArrayInit>(values), &arrayDS);
                }
            }
        } else if (selectedOperation == 1) { // Insert
            ImGui::Text("Insert Operation:");
            ImGui::InputInt("Index##insert", &insertIndex);
            ImGui::InputInt("Value##insert", &insertValue);
            if (ImGui::Button("Execute Insert")) {
                if (insertIndex >= 0 && insertIndex <= (int)arrayDS.size()) {
                    controller.stageOperation(std::make_unique<ArrayInsert>(arrayDS, insertIndex, insertValue), &arrayDS);
                }
            }
        } else if (selectedOperation == 2) { // Delete
            ImGui::Text("Delete Operation:");
            ImGui::InputInt("Index##delete", &deleteIndex);
            if (ImGui::Button("Execute Delete")) {
                if (deleteIndex >= 0 && deleteIndex < (int)arrayDS.size()) {
                    controller.stageOperation(std::make_unique<ArrayDelete>(arrayDS, deleteIndex), &arrayDS);
                }
            }
        }
    }
};
