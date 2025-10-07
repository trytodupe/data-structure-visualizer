#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "../data_structure/StackStructure.h"
#include "../operation/StackOps.h"
#include "../core/OperationManager.h"
#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <cstring>

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
    char initValuesInput[256];  // Buffer for custom init values

public:
    StackVisualizerPanel()
        : pushValue(10), selectedOperation(0) {
        // Default init values
        strcpy(initValuesInput, "1, 2, 3, 4, 5");
    }

    /**
     * Render the complete stack panel (visualization + controls)
     */
    void render(StackStructure& stackDS, VisualizationController& controller, OperationManager& opManager) {
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
        renderOperationControls(stackDS, controller, opManager);
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
    void renderOperationControls(StackStructure& stackDS, VisualizationController& controller, OperationManager& opManager) {
        ImGui::Text("Select Stack Operation:");
        const char* stackOps[] = { "Initialize", "Push", "Pop"};
        ImGui::Combo("##stackOperation", &selectedOperation, stackOps, IM_ARRAYSIZE(stackOps));
        ImGui::Spacing();

        if (selectedOperation == 0) { // Initialize
            ImGui::Text("Initialize Operation:");
            ImGui::Text("Initialize stack with custom values (comma-separated)");
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
                    controller.stageOperation(std::make_unique<StackInit>(stackDS, values), &stackDS);
                }
            }
        } else if (selectedOperation == 1) { // Push
            ImGui::Text("Push Operation:");
            ImGui::InputInt("Value##push", &pushValue);
            if (ImGui::Button("Execute Push")) {
                controller.stageOperation(std::make_unique<StackPush>(pushValue), &stackDS);
            }
        } else if (selectedOperation == 2) { // Pop
            ImGui::Text("Pop Operation:");
            ImGui::Text("Removes the top element from the stack.");
            if (ImGui::Button("Execute Pop")) {
                if (!stackDS.data.empty()) {
                    controller.stageOperation(std::make_unique<StackPop>(), &stackDS);
                }
            }
        }
    }
};
