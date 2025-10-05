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

        // Display current array state visually
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Array:");
        ImGui::Text("Size: %zu", arrayDS.data.size());
        ImGui::Spacing();

        // Visual representation of array
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        float box_size = 60.0f;
        float spacing = 5.0f;

        for (size_t i = 0; i < arrayDS.data.size(); ++i) {
            ImVec2 box_min = ImVec2(canvas_pos.x + i * (box_size + spacing), canvas_pos.y);
            ImVec2 box_max = ImVec2(box_min.x + box_size, box_min.y + box_size);

            // Draw box
            draw_list->AddRectFilled(box_min, box_max, IM_COL32(70, 130, 180, 255));
            draw_list->AddRect(box_min, box_max, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

            // Draw value text centered
            char value_text[32];
            snprintf(value_text, sizeof(value_text), "%d", arrayDS.data[i]);
            ImVec2 text_size = ImGui::CalcTextSize(value_text);
            ImVec2 text_pos = ImVec2(
                box_min.x + (box_size - text_size.x) * 0.5f,
                box_min.y + (box_size - text_size.y) * 0.5f
            );
            draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), value_text);

            // Draw index below box
            char index_text[32];
            snprintf(index_text, sizeof(index_text), "[%zu]", i);
            ImVec2 index_size = ImGui::CalcTextSize(index_text);
            ImVec2 index_pos = ImVec2(
                box_min.x + (box_size - index_size.x) * 0.5f,
                box_max.y + 5.0f
            );
            draw_list->AddText(index_pos, IM_COL32(200, 200, 200, 255), index_text);
        }

        // Reserve space for the visual representation
        ImGui::Dummy(ImVec2(arrayDS.data.size() * (box_size + spacing), box_size + 30.0f));
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

        // Display current stack state visually
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Stack:");
        ImGui::Text("Size: %zu", stackDS.data.size());
        ImGui::Spacing();

        // Get stack elements (top to bottom)
        std::stack<int> tempStack = stackDS.data;
        std::vector<int> stackElements;
        while (!tempStack.empty()) {
            stackElements.push_back(tempStack.top());
            tempStack.pop();
        }

        // Visual representation of stack
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        float box_width = 150.0f;
        float box_height = 40.0f;
        float spacing = 5.0f;

        if (stackElements.empty()) {
            // Draw empty stack container
            ImVec2 empty_min = ImVec2(canvas_pos.x, canvas_pos.y);
            ImVec2 empty_max = ImVec2(empty_min.x + box_width, empty_min.y + 100.0f);
            draw_list->AddRect(empty_min, empty_max, IM_COL32(150, 150, 150, 255), 0.0f, 0, 2.0f);

            const char* empty_text = "(empty)";
            ImVec2 text_size = ImGui::CalcTextSize(empty_text);
            ImVec2 text_pos = ImVec2(
                empty_min.x + (box_width - text_size.x) * 0.5f,
                empty_min.y + (100.0f - text_size.y) * 0.5f
            );
            draw_list->AddText(text_pos, IM_COL32(150, 150, 150, 255), empty_text);
            ImGui::Dummy(ImVec2(box_width, 100.0f));
        } else {
            // Draw stack elements from top to bottom
            for (size_t i = 0; i < stackElements.size(); ++i) {
                ImVec2 box_min = ImVec2(canvas_pos.x, canvas_pos.y + i * (box_height + spacing));
                ImVec2 box_max = ImVec2(box_min.x + box_width, box_min.y + box_height);

                // Different color for top element
                ImU32 box_color = (i == 0) ? IM_COL32(220, 100, 100, 255) : IM_COL32(100, 180, 100, 255);
                draw_list->AddRectFilled(box_min, box_max, box_color);
                draw_list->AddRect(box_min, box_max, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

                // Draw value text centered
                char value_text[32];
                snprintf(value_text, sizeof(value_text), "%d", stackElements[i]);
                ImVec2 text_size = ImGui::CalcTextSize(value_text);
                ImVec2 text_pos = ImVec2(
                    box_min.x + (box_width - text_size.x) * 0.5f,
                    box_min.y + (box_height - text_size.y) * 0.5f
                );
                draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), value_text);

                // Label "TOP" for first element
                if (i == 0) {
                    const char* top_text = "TOP";
                    ImVec2 top_text_pos = ImVec2(box_max.x + 10.0f, box_min.y + (box_height - ImGui::CalcTextSize(top_text).y) * 0.5f);
                    draw_list->AddText(top_text_pos, IM_COL32(220, 100, 100, 255), top_text);
                }
            }

            // Reserve space for the visual representation
            ImGui::Dummy(ImVec2(box_width + 60.0f, stackElements.size() * (box_height + spacing)));
        }

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
