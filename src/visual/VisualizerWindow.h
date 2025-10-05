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

    // Dropdown selection state
    int selectedArrayOp;
    int selectedStackOp;

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
          selectedArrayOp(0), selectedStackOp(0),
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

        // Determine if arrow should be shown (only when not in visualization phase)
        bool showArrow = false;
        int arrowIndex = -1;
        ImU32 arrowColor = IM_COL32(255, 200, 0, 255); // Orange/yellow color

        if (!isVisualizing) {
            if (selectedArrayOp == 0 && insertIndex >= 0 && insertIndex <= (int)arrayDS.data.size()) {
                // Insert operation - show arrow at insert position
                showArrow = true;
                arrowIndex = insertIndex;
                arrowColor = IM_COL32(0, 255, 100, 255); // Green for insert
            } else if (selectedArrayOp == 1 && deleteIndex >= 0 && deleteIndex < (int)arrayDS.data.size()) {
                // Delete operation - show arrow at delete position
                showArrow = true;
                arrowIndex = deleteIndex;
                arrowColor = IM_COL32(255, 100, 100, 255); // Red for delete
            }
        }

        for (size_t i = 0; i < arrayDS.data.size(); ++i) {
            ImVec2 box_min = ImVec2(canvas_pos.x + i * (box_size + spacing), canvas_pos.y);
            ImVec2 box_max = ImVec2(box_min.x + box_size, box_min.y + box_size);

            // Highlight box if it's the selected index
            bool isSelected = (showArrow && arrowIndex == (int)i);
            ImU32 boxColor = isSelected ?
                (selectedArrayOp == 0 ? IM_COL32(50, 200, 120, 255) : IM_COL32(200, 80, 80, 255)) :
                IM_COL32(70, 130, 180, 255);

            // Draw box
            draw_list->AddRectFilled(box_min, box_max, boxColor);
            draw_list->AddRect(box_min, box_max, IM_COL32(255, 255, 255, 255), 0.0f, 0, isSelected ? 3.0f : 2.0f);

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
            ImU32 indexColor = isSelected ? arrowColor : IM_COL32(200, 200, 200, 255);
            draw_list->AddText(index_pos, indexColor, index_text);
        }

        // Draw arrow below selected index for Insert/Delete operations (after boxes)
        if (showArrow && arrowIndex >= 0) {
            // Calculate arrow position below the boxes
            float arrow_x = canvas_pos.x + arrowIndex * (box_size + spacing) + box_size * 0.5f;
            float arrow_base_y = canvas_pos.y + box_size + 30.0f; // Below index labels

            // Draw arrow pointing up
            ImVec2 arrow_tip = ImVec2(arrow_x, canvas_pos.y + box_size + 25.0f);
            ImVec2 arrow_left = ImVec2(arrow_x - 8.0f, arrow_base_y);
            ImVec2 arrow_right = ImVec2(arrow_x + 8.0f, arrow_base_y);
            ImVec2 arrow_shaft_bottom = ImVec2(arrow_x, arrow_base_y + 15.0f);

            // Draw arrow shaft (vertical line)
            draw_list->AddLine(arrow_shaft_bottom, ImVec2(arrow_x, arrow_base_y), arrowColor, 3.0f);
            // Draw arrow head (triangle pointing up)
            draw_list->AddTriangleFilled(arrow_tip, arrow_left, arrow_right, arrowColor);

            // Draw label below arrow
            const char* label = (selectedArrayOp == 0) ? "Insert here" : "Delete this";
            ImVec2 label_size = ImGui::CalcTextSize(label);
            ImVec2 label_pos = ImVec2(arrow_x - label_size.x * 0.5f, arrow_shaft_bottom.y + 5.0f);
            draw_list->AddText(label_pos, arrowColor, label);
        }

        // Draw insertion position indicator for Insert at end
        if (showArrow && selectedArrayOp == 0 && insertIndex == (int)arrayDS.data.size()) {
            // Show arrow after last element
            float arrow_x = canvas_pos.x + arrayDS.data.size() * (box_size + spacing) + box_size * 0.5f;
            float arrow_base_y = canvas_pos.y + box_size + 30.0f;

            ImVec2 arrow_tip = ImVec2(arrow_x, canvas_pos.y + box_size + 25.0f);
            ImVec2 arrow_left = ImVec2(arrow_x - 8.0f, arrow_base_y);
            ImVec2 arrow_right = ImVec2(arrow_x + 8.0f, arrow_base_y);
            ImVec2 arrow_shaft_bottom = ImVec2(arrow_x, arrow_base_y + 15.0f);

            draw_list->AddLine(arrow_shaft_bottom, ImVec2(arrow_x, arrow_base_y), IM_COL32(0, 255, 100, 255), 3.0f);
            draw_list->AddTriangleFilled(arrow_tip, arrow_left, arrow_right, IM_COL32(0, 255, 100, 255));

            const char* label = "Insert here";
            ImVec2 label_size = ImGui::CalcTextSize(label);
            ImVec2 label_pos = ImVec2(arrow_x - label_size.x * 0.5f, arrow_shaft_bottom.y + 5.0f);
            draw_list->AddText(label_pos, IM_COL32(0, 255, 100, 255), label);
        }

        // Reserve space for the visual representation (increased for arrow below)
        ImGui::Dummy(ImVec2((arrayDS.data.size() + 1) * (box_size + spacing), box_size + 80.0f));
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Operation selection dropdown
        ImGui::Text("Select Array Operation:");
        const char* arrayOps[] = { "Insert", "Delete", "Sort", "Reset" };
        ImGui::Combo("##arrayOperation", &selectedArrayOp, arrayOps, IM_ARRAYSIZE(arrayOps));
        ImGui::Spacing();

        // Show inputs based on selected operation
        if (selectedArrayOp == 0) { // Insert
            ImGui::Text("Insert Operation:");
            ImGui::InputInt("Index##insert", &insertIndex);
            ImGui::InputInt("Value##insert", &insertValue);
            if (ImGui::Button("Execute Insert")) {
                if (insertIndex >= 0 && insertIndex <= (int)arrayDS.data.size()) {
                    stageOperation(std::make_unique<ArrayInsert>(arrayDS, insertIndex, insertValue), &arrayDS);
                }
            }
        } else if (selectedArrayOp == 1) { // Delete
            ImGui::Text("Delete Operation:");
            ImGui::InputInt("Index##delete", &deleteIndex);
            if (ImGui::Button("Execute Delete")) {
                if (deleteIndex >= 0 && deleteIndex < (int)arrayDS.data.size()) {
                    stageOperation(std::make_unique<ArrayDelete>(arrayDS, deleteIndex), &arrayDS);
                }
            }
        } else if (selectedArrayOp == 2) { // Sort
            ImGui::Text("Sort Operation:");
            ImGui::Text("Sorts the array using Bubble Sort algorithm.");
            if (ImGui::Button("Execute Sort")) {
                stageOperation(std::make_unique<ArraySort>(arrayDS), &arrayDS);
            }
        } else if (selectedArrayOp == 3) { // Reset
            ImGui::Text("Reset Operation:");
            ImGui::Text("Resets array to [5, 2, 8, 1, 9].");
            if (ImGui::Button("Execute Reset")) {
                std::vector<int> resetValues = {5, 2, 8, 1, 9};
                stageOperation(std::make_unique<ArrayInit>(resetValues), &arrayDS);
            }
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

        // Operation selection dropdown
        ImGui::Text("Select Stack Operation:");
        const char* stackOps[] = { "Push", "Pop", "Clear", "Initialize" };
        ImGui::Combo("##stackOperation", &selectedStackOp, stackOps, IM_ARRAYSIZE(stackOps));
        ImGui::Spacing();

        // Show inputs based on selected operation
        if (selectedStackOp == 0) { // Push
            ImGui::Text("Push Operation:");
            ImGui::InputInt("Value##push", &pushValue);
            if (ImGui::Button("Execute Push")) {
                stageOperation(std::make_unique<StackPush>(pushValue), &stackDS);
            }
        } else if (selectedStackOp == 1) { // Pop
            ImGui::Text("Pop Operation:");
            ImGui::Text("Removes the top element from the stack.");
            if (ImGui::Button("Execute Pop")) {
                if (!stackDS.data.empty()) {
                    stageOperation(std::make_unique<StackPop>(), &stackDS);
                }
            }
        } else if (selectedStackOp == 2) { // Clear
            ImGui::Text("Clear Operation:");
            ImGui::Text("Removes all elements from the stack.");
            if (ImGui::Button("Execute Clear")) {
                stageOperation(std::make_unique<StackClear>(stackDS), &stackDS);
            }
        } else if (selectedStackOp == 3) { // Initialize
            ImGui::Text("Initialize Operation:");
            ImGui::Text("Initializes stack with [1, 2, 3, 4, 5].");
            if (ImGui::Button("Execute Initialize")) {
                std::vector<int> values = {1, 2, 3, 4, 5};
                stageOperation(std::make_unique<StackInit>(values), &stackDS);
            }
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
