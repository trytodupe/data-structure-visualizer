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
        // Use 'null' or 'x' for missing nodes
        strcpy(initValuesInput, "10, 5, 15, 3, null, 12, 20");
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
            ImGui::Text("Initialize tree with values (comma-separated, level-order)");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Example: 10, 5, 15, null, 7, 12, x");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Use 'null' or 'x' for missing nodes");
            ImGui::Spacing();
            ImGui::InputText("Values##init", initValuesInput, IM_ARRAYSIZE(initValuesInput));
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: This will clear undo/redo history");

            if (ImGui::Button("Execute Initialize")) {
                // Parse the input string to extract values (with null support)
                std::vector<std::pair<bool, int>> values; // pair<hasValue, value>
                std::string input(initValuesInput);
                std::string current = "";

                auto processToken = [&values](const std::string& token) {
                    // Check if token is null/x (case-insensitive)
                    std::string lower = token;
                    for (char& c : lower) c = std::tolower(c);

                    if (lower == "null" || lower == "x" || lower == "none") {
                        values.push_back({false, 0}); // null node
                    } else {
                        try {
                            int val = std::stoi(token);
                            values.push_back({true, val}); // actual value
                        } catch (...) {
                            // Skip invalid tokens
                        }
                    }
                };

                for (char c : input) {
                    if (c == ',') {
                        if (!current.empty()) {
                            processToken(current);
                            current = "";
                        }
                    } else if (c != ' ' && c != '\t') {
                        current += c;
                    }
                }
                // Don't forget the last value
                if (!current.empty()) {
                    processToken(current);
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
