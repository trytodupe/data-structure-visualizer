#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "../data_structure/StackStructure.h"
#include "../operation/StackOps.h"
#include <stack>
#include <vector>
#include <memory>

/**
 * Panel for stack visualization and operation controls.
 * Handles rendering stack visual representation, operation selection,
 * and user inputs for stack operations.
 */
class StackVisualizerPanel {
private:
    // Input state
    int pushValue;
    int selectedOperation;

public:
    StackVisualizerPanel()
        : pushValue(10), selectedOperation(0) {}

    /**
     * Render the complete stack panel (visualization + controls)
     */
    void render(StackStructure& stackDS, VisualizationController& controller) {
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

        // Check if top is being modified
        bool topBeingModified = isTopBeingModified(controller);

        // Render visual representation
        renderVisual(stackElements, topBeingModified);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Render operation controls
        renderOperationControls(stackDS, controller);
    }

private:
    /**
     * Check if top element is being modified in current atomic operation
     */
    bool isTopBeingModified(VisualizationController& controller) {
        if (!controller.isInVisualizationPhase()) {
            return false;
        }

        Operation* currentOp = controller.getCurrentAtomicOperation();
        if (!currentOp) {
            return false;
        }

        // Check if it's a PushOp or PopOp
        return (dynamic_cast<PushOp*>(currentOp) != nullptr ||
                dynamic_cast<PopOp*>(currentOp) != nullptr);
    }

    /**
     * Render the visual representation of the stack
     */
    void renderVisual(const std::vector<int>& stackElements, bool topBeingModified) {
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

                // Determine box color and border
                ImU32 box_color;
                float borderThickness = 2.0f;

                if (i == 0 && topBeingModified) {
                    // Bright orange for top element being modified
                    box_color = IM_COL32(255, 180, 0, 255);
                    borderThickness = 4.0f;
                } else if (i == 0) {
                    // Red for top element
                    box_color = IM_COL32(220, 100, 100, 255);
                } else {
                    // Green for other elements
                    box_color = IM_COL32(100, 180, 100, 255);
                }

                draw_list->AddRectFilled(box_min, box_max, box_color);
                draw_list->AddRect(box_min, box_max, IM_COL32(255, 255, 255, 255), 0.0f, 0, borderThickness);

                // Draw value text
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
                    ImVec2 top_text_pos = ImVec2(box_max.x + 10.0f,
                        box_min.y + (box_height - ImGui::CalcTextSize(top_text).y) * 0.5f);
                    draw_list->AddText(top_text_pos, IM_COL32(220, 100, 100, 255), top_text);
                }
            }

            // Reserve space
            ImGui::Dummy(ImVec2(box_width + 60.0f, stackElements.size() * (box_height + spacing)));
        }
    }

    /**
     * Render operation selection and controls
     */
    void renderOperationControls(StackStructure& stackDS, VisualizationController& controller) {
        ImGui::Text("Select Stack Operation:");
        const char* stackOps[] = { "Push", "Pop", "Clear", "Initialize" };
        ImGui::Combo("##stackOperation", &selectedOperation, stackOps, IM_ARRAYSIZE(stackOps));
        ImGui::Spacing();

        if (selectedOperation == 0) { // Push
            ImGui::Text("Push Operation:");
            ImGui::InputInt("Value##push", &pushValue);
            if (ImGui::Button("Execute Push")) {
                controller.stageOperation(std::make_unique<StackPush>(pushValue), &stackDS);
            }
        } else if (selectedOperation == 1) { // Pop
            ImGui::Text("Pop Operation:");
            ImGui::Text("Removes the top element from the stack.");
            if (ImGui::Button("Execute Pop")) {
                if (!stackDS.data.empty()) {
                    controller.stageOperation(std::make_unique<StackPop>(), &stackDS);
                }
            }
        } else if (selectedOperation == 2) { // Clear
            ImGui::Text("Clear Operation:");
            ImGui::Text("Removes all elements from the stack.");
            if (ImGui::Button("Execute Clear")) {
                controller.stageOperation(std::make_unique<StackClear>(stackDS), &stackDS);
            }
        } else if (selectedOperation == 3) { // Initialize
            ImGui::Text("Initialize Operation:");
            ImGui::Text("Initializes stack with [1, 2, 3, 4, 5].");
            if (ImGui::Button("Execute Initialize")) {
                std::vector<int> values = {1, 2, 3, 4, 5};
                controller.stageOperation(std::make_unique<StackInit>(values), &stackDS);
            }
        }
    }
};
