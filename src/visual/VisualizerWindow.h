#pragma once
#include "imgui.h"
#include "../core/OperationManager.h"
#include "../data_structure/ArrayStructure.h"
#include "../data_structure/StackStructure.h"
#include "../operation/ArrayOps.h"
#include "../operation/StackOps.h"
#include <memory>
#include <string>
#include <stack>

/**
 * Main visualizer window for data structure operations
 */
class VisualizerWindow {
private:
    bool isOpen;
    float windowScale;

    // Demo control state
    int insertIndex;
    int insertValue;
    int deleteIndex;
    int pushValue;

    // Visualization state
    std::unique_ptr<UserOperation> stagedOperation;
    DataStructure* stagedDataStructure;
    size_t currentAtomicStep;
    bool isVisualizing;

public:
    VisualizerWindow(float scale = 1.0f)
        : isOpen(true), windowScale(scale),
          insertIndex(0), insertValue(99),
          deleteIndex(0), pushValue(10),
          stagedOperation(nullptr), stagedDataStructure(nullptr),
          currentAtomicStep(0), isVisualizing(false) {}

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

        // Visualization Control Button
        renderVisualizationControl(arrayDS, stackDS, opManager);
        ImGui::Separator();

        // Tabs for different data structures
        if (ImGui::BeginTabBar("DataStructureTabs"))
        {
            // === ARRAY TAB ===
            if (ImGui::BeginTabItem("Array"))
            {
                renderArrayTab(arrayDS, opManager);
                ImGui::EndTabItem();
            }

            // === STACK TAB ===
            if (ImGui::BeginTabItem("Stack"))
            {
                renderStackTab(stackDS, opManager);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

private:
    /**
     * Render the Array tab contents
     */
    void renderArrayTab(ArrayStructure& arrayDS, OperationManager& opManager) {
        ImGui::Text("Array Operations:");
        ImGui::Spacing();

        // Display current array state
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Array:");
        std::string arrayStr = "[ ";
        for (size_t i = 0; i < arrayDS.data.size(); ++i) {
            arrayStr += std::to_string(arrayDS.data[i]);
            if (i < arrayDS.data.size() - 1) arrayStr += ", ";
        }
        arrayStr += " ]";
        ImGui::Text("%s", arrayStr.c_str());
        ImGui::Text("Size: %zu", arrayDS.data.size());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Insert Operation
        ImGui::Text("Insert Operation:");
        ImGui::InputInt("Index##insert", &insertIndex);
        ImGui::InputInt("Value##insert", &insertValue);
        if (ImGui::Button("Insert Element")) {
            if (insertIndex >= 0 && insertIndex <= (int)arrayDS.data.size()) {
                stageOperation(std::make_unique<ArrayInsert>(arrayDS, insertIndex, insertValue), &arrayDS);
            }
        }
        ImGui::Spacing();

        // Delete Operation
        ImGui::Text("Delete Operation:");
        ImGui::InputInt("Index##delete", &deleteIndex);
        if (ImGui::Button("Delete Element")) {
            if (deleteIndex >= 0 && deleteIndex < (int)arrayDS.data.size()) {
                stageOperation(std::make_unique<ArrayDelete>(arrayDS, deleteIndex), &arrayDS);
            }
        }
        ImGui::Spacing();

        // Sort Operation
        if (ImGui::Button("Sort Array (Bubble Sort)")) {
            stageOperation(std::make_unique<ArraySort>(arrayDS), &arrayDS);
        }
        ImGui::Spacing();

        // Reset Operation
        if (ImGui::Button("Reset Array")) {
            std::vector<int> resetValues = {5, 2, 8, 1, 9};
            stageOperation(std::make_unique<ArrayInit>(resetValues), &arrayDS);
        }
    }

    /**
     * Render the Stack tab contents
     */
    void renderStackTab(StackStructure& stackDS, OperationManager& opManager) {
        ImGui::Text("Stack Operations:");
        ImGui::Spacing();

        // Display current stack state
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Stack:");
        std::stack<int> tempStack = stackDS.data;
        std::vector<int> stackElements;
        while (!tempStack.empty()) {
            stackElements.push_back(tempStack.top());
            tempStack.pop();
        }

        if (stackElements.empty()) {
            ImGui::Text("(empty)");
        } else {
            ImGui::Text("Top -> [ ");
            for (int val : stackElements) {
                ImGui::SameLine();
                ImGui::Text("%d ", val);
            }
            ImGui::SameLine();
            ImGui::Text("]");
        }
        ImGui::Text("Size: %zu", stackDS.data.size());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Push Operation
        ImGui::Text("Push Operation:");
        ImGui::InputInt("Value##push", &pushValue);
        if (ImGui::Button("Push to Stack")) {
            stageOperation(std::make_unique<StackPush>(pushValue), &stackDS);
        }
        ImGui::Spacing();

        // Pop Operation
        if (ImGui::Button("Pop from Stack")) {
            if (!stackDS.data.empty()) {
                stageOperation(std::make_unique<StackPop>(), &stackDS);
            }
        }
        ImGui::Spacing();

        // Clear Operation
        if (ImGui::Button("Clear Stack")) {
            stageOperation(std::make_unique<StackClear>(stackDS), &stackDS);
        }
        ImGui::Spacing();

        // Initialize Stack
        if (ImGui::Button("Initialize Stack [1,2,3,4,5]")) {
            std::vector<int> values = {1, 2, 3, 4, 5};
            stageOperation(std::make_unique<StackInit>(values), &stackDS);
        }
    }

    /**
     * Stage an operation for step-by-step visualization
     */
    void stageOperation(std::unique_ptr<UserOperation> op, DataStructure* ds) {
        stagedOperation = std::move(op);
        stagedDataStructure = ds;
        currentAtomicStep = 0;
        isVisualizing = false;
    }

    /**
     * Render the visualization control button (Start/Step)
     */
    void renderVisualizationControl(ArrayStructure& arrayDS, StackStructure& stackDS, OperationManager& opManager) {
        if (!stagedOperation) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No operation staged. Click an operation button to stage it.");
            return;
        }

        // Show staged operation info
        ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Staged Operation: %s", stagedOperation->getName().c_str());
        ImGui::Text("Total atomic operations: %zu", stagedOperation->operations.size());

        if (isVisualizing) {
            ImGui::Text("Current step: %zu / %zu", currentAtomicStep, stagedOperation->operations.size());
        }
        ImGui::Spacing();

        // Start or Step button
        if (!isVisualizing) {
            if (ImGui::Button("Start Visualization")) {
                isVisualizing = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                stagedOperation.reset();
                stagedDataStructure = nullptr;
            }
        } else {
            if (currentAtomicStep < stagedOperation->operations.size()) {
                if (ImGui::Button("Step (Execute Next Atomic Operation)")) {
                    // Execute the current atomic operation
                    stagedOperation->operations[currentAtomicStep]->execute(*stagedDataStructure);
                    currentAtomicStep++;

                    // If we've finished all steps, add to history
                    if (currentAtomicStep >= stagedOperation->operations.size()) {
                        opManager.executeOperation(*stagedDataStructure, std::move(stagedOperation));
                        stagedOperation.reset();
                        stagedDataStructure = nullptr;
                        isVisualizing = false;
                        currentAtomicStep = 0;
                    }
                }
            }
        }
    }
};
