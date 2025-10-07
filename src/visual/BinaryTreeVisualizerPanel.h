#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "../data_structure/BinaryTreeStructure.h"
#include "../operation/BinaryTreeOps.h"
#include "../core/OperationManager.h"
#include <vector>
#include <memory>
#include <string>
#include <cstring>

/**
 * Panel for binary tree visualization and operation controls.
 * Handles rendering tree visual representation, operation selection,
 * and user inputs for tree operations.
 */
class BinaryTreeVisualizerPanel {
private:
    // Input state
    int selectedOperation;
    char initValuesInput[256];  // Buffer for custom init values

public:
    BinaryTreeVisualizerPanel()
        : selectedOperation(0) {
        // Default init values (level-order: root, left child, right child, etc.)
        strcpy(initValuesInput, "10, 5, 15, 3, 7, 12, 20");
    }

    /**
     * Render the complete binary tree panel (visualization + controls)
     */
    void render(BinaryTreeStructure& treeDS, VisualizationController& controller, OperationManager& opManager) {
        ImGui::Text("Binary Tree Operations:");
        ImGui::Spacing();

        // Display current tree state
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Tree:");
        ImGui::Text("Nodes: %zu", treeDS.nodeCount);
        if (treeDS.root) {
            ImGui::Text("Root: %d", treeDS.root->value);
        } else {
            ImGui::Text("Root: (empty)");
        }
        ImGui::Spacing();

        // Render visual representation
        renderVisual(treeDS, controller);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Render operation controls
        renderOperationControls(treeDS, controller, opManager);
    }

private:
    /**
     * Render the visual representation of the binary tree
     */
    void renderVisual(BinaryTreeStructure& treeDS, VisualizationController& controller) {
        const float boxSize = 40.0f;
        const float spacing = 10.0f;
        ImVec2 startPos = ImGui::GetCursorScreenPos();

        // 1. Draw the data structure base state
        ImVec2 drawnSize = treeDS.draw(startPos, boxSize, spacing);

        // 2. Draw operation overlay if visualizing
        if (controller.isInVisualizationPhase()) {
            const auto* currentOp = controller.getCurrentAtomicOperation();
            if (currentOp) {
                currentOp->drawOverlay(treeDS, startPos, boxSize, spacing);
            }
        }

        // 3. Reserve space for the drawn area
        ImGui::Dummy(drawnSize);
    }

    /**
     * Render operation selection and controls
     */
    void renderOperationControls(BinaryTreeStructure& treeDS, VisualizationController& controller, OperationManager& opManager) {
        ImGui::Text("Select Binary Tree Operation:");
        const char* treeOps[] = { "Initialize" };
        ImGui::Combo("##treeOperation", &selectedOperation, treeOps, IM_ARRAYSIZE(treeOps));
        ImGui::Spacing();

        if (selectedOperation == 0) { // Initialize
            ImGui::Text("Initialize Operation:");
            ImGui::Text("Initialize tree with custom values (comma-separated, level-order)");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Example: 10, 5, 15, 3, 7, 12, 20");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Creates complete binary tree level by level");
            ImGui::Spacing();
            ImGui::InputText("Values##init", initValuesInput, IM_ARRAYSIZE(initValuesInput));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: This will clear undo/redo history");

            if (ImGui::Button("Execute Initialize")) {
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
                    opManager.clear();
                    controller.stageOperation(std::make_unique<BinaryTreeInit>(treeDS, values), &treeDS);
                }
            }
        }
    }
};
