#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "ArrayVisualizerPanel.h"
#include "StackVisualizerPanel.h"
#include "BinaryTreeVisualizerPanel.h"
#include "../core/OperationManager.h"
#include "../data_structure/ArrayStructure.h"
#include "../data_structure/StackStructure.h"
#include "../data_structure/BinaryTreeStructure.h"

/**
 * Main visualizer window for data structure operations.
 * Coordinates ArrayVisualizerPanel, StackVisualizerPanel, BinaryTreeVisualizerPanel, and VisualizationController.
 */
class VisualizerWindow {
private:
    bool isOpen;
    float windowScale;

    // Visualization components
    VisualizationController vizController;
    ArrayVisualizerPanel arrayPanel;
    StackVisualizerPanel stackPanel;
    BinaryTreeVisualizerPanel treePanel;

public:
    VisualizerWindow(float scale = 1.0f)
        : isOpen(true), windowScale(scale) {}

    /**
     * Show/hide the visualizer window
     */
    void setOpen(bool open) {
        isOpen = open;
    }

    bool getOpen() const {
        return isOpen;
    }

    /**
     * Render the visualizer window
     * @param arrayDS Array data structure reference
     * @param stackDS Stack data structure reference
     * @param treeDS Binary tree data structure reference
     * @param opManager Operation manager reference
     */
    void render(ArrayStructure& arrayDS, StackStructure& stackDS, BinaryTreeStructure& treeDS, OperationManager& opManager) {
        if (!isOpen) return;

        ImGui::SetNextWindowSize(ImVec2(1200 * windowScale, 800 * windowScale), ImGuiCond_FirstUseEver);
        ImGui::Begin("Data Structure Visualizer", &isOpen);

        ImGui::Text("Interactive Data Structure Operations Demo");
        ImGui::Separator();

        // Visualization Control
        vizController.renderControls(opManager);
        ImGui::Separator();

        // Tabs for different data structures
        if (ImGui::BeginTabBar("DataStructureTabs"))
        {
            // === ARRAY TAB ===
            if (ImGui::BeginTabItem("Array"))
            {
                arrayPanel.render(arrayDS, vizController, opManager);
                ImGui::EndTabItem();
            }

            // === STACK TAB ===
            if (ImGui::BeginTabItem("Stack"))
            {
                stackPanel.render(stackDS, vizController, opManager);
                ImGui::EndTabItem();
            }

            // === BINARY TREE TAB ===
            if (ImGui::BeginTabItem("Binary Tree"))
            {
                treePanel.render(treeDS, vizController, opManager);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
};
