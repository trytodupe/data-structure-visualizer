#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "ArrayVisualizerPanel.h"
#include "StackVisualizerPanel.h"
#include "../core/OperationManager.h"
#include "../data_structure/ArrayStructure.h"
#include "../data_structure/StackStructure.h"

/**
 * Main visualizer window for data structure operations.
 * Coordinates ArrayVisualizerPanel, StackVisualizerPanel, and VisualizationController.
 */
class VisualizerWindow {
private:
    bool isOpen;
    float windowScale;

    // Visualization components
    VisualizationController vizController;
    ArrayVisualizerPanel arrayPanel;
    StackVisualizerPanel stackPanel;

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
     * @param opManager Operation manager reference
     */
    void render(ArrayStructure& arrayDS, StackStructure& stackDS, OperationManager& opManager) {
        if (!isOpen) return;

        ImGui::SetNextWindowSize(ImVec2(1000 * windowScale, 700 * windowScale), ImGuiCond_FirstUseEver);
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
                stackPanel.render(stackDS, vizController);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
};
