#pragma once
#include "imgui.h"
#include "VisualizationController.h"
#include "../data_structure/ArrayStructure.h"
#include "../operation/ArrayOps.h"
#include <set>
#include <memory>

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

public:
    ArrayVisualizerPanel()
        : insertIndex(0), insertValue(99),
          deleteIndex(0), selectedOperation(0) {}

    /**
     * Render the complete array panel (visualization + controls)
     */
    void render(ArrayStructure& arrayDS, VisualizationController& controller) {
        ImGui::Text("Array Operations:");
        ImGui::Spacing();

        // Display current array state visually
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Array:");
        ImGui::Text("Size: %zu", arrayDS.data.size());
        ImGui::Spacing();

        // Get modified indices for highlighting
        std::set<size_t> modifiedIndices = getModifiedIndices(controller);

        // Render visual representation
        renderVisual(arrayDS, controller, modifiedIndices);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Render operation controls
        renderOperationControls(arrayDS, controller);
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
    void renderVisual(ArrayStructure& arrayDS, VisualizationController& controller,
                     const std::set<size_t>& modifiedIndices) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        float box_size = 60.0f;
        float spacing = 5.0f;

        // Determine if arrow should be shown (only when not in visualization phase)
        bool showArrow = false;
        int arrowIndex = -1;
        ImU32 arrowColor = IM_COL32(255, 200, 0, 255);

        if (!controller.isInVisualizationPhase()) {
            if (selectedOperation == 0 && insertIndex >= 0 && insertIndex <= (int)arrayDS.data.size()) {
                showArrow = true;
                arrowIndex = insertIndex;
                arrowColor = IM_COL32(0, 255, 100, 255); // Green for insert
            } else if (selectedOperation == 1 && deleteIndex >= 0 && deleteIndex < (int)arrayDS.data.size()) {
                showArrow = true;
                arrowIndex = deleteIndex;
                arrowColor = IM_COL32(255, 100, 100, 255); // Red for delete
            }
        }

        // Draw array boxes
        for (size_t i = 0; i < arrayDS.data.size(); ++i) {
            ImVec2 box_min = ImVec2(canvas_pos.x + i * (box_size + spacing), canvas_pos.y);
            ImVec2 box_max = ImVec2(box_min.x + box_size, box_min.y + box_size);

            bool isSelected = (showArrow && arrowIndex == (int)i);
            bool isBeingModified = (modifiedIndices.find(i) != modifiedIndices.end());

            ImU32 boxColor;
            if (isBeingModified) {
                boxColor = IM_COL32(255, 180, 0, 255); // Orange for modified
            } else if (isSelected) {
                boxColor = (selectedOperation == 0 ? IM_COL32(50, 200, 120, 255) : IM_COL32(200, 80, 80, 255));
            } else {
                boxColor = IM_COL32(70, 130, 180, 255); // Default blue
            }

            draw_list->AddRectFilled(box_min, box_max, boxColor);
            float borderThickness = isBeingModified ? 4.0f : (isSelected ? 3.0f : 2.0f);
            draw_list->AddRect(box_min, box_max, IM_COL32(255, 255, 255, 255), 0.0f, 0, borderThickness);

            // Draw value text
            char value_text[32];
            snprintf(value_text, sizeof(value_text), "%d", arrayDS.data[i]);
            ImVec2 text_size = ImGui::CalcTextSize(value_text);
            ImVec2 text_pos = ImVec2(
                box_min.x + (box_size - text_size.x) * 0.5f,
                box_min.y + (box_size - text_size.y) * 0.5f
            );
            draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), value_text);

            // Draw index label
            char index_text[32];
            snprintf(index_text, sizeof(index_text), "[%zu]", i);
            ImVec2 index_size = ImGui::CalcTextSize(index_text);
            ImVec2 index_pos = ImVec2(
                box_min.x + (box_size - index_size.x) * 0.5f,
                box_max.y + 5.0f
            );
            ImU32 indexColor = isBeingModified ? IM_COL32(255, 180, 0, 255) :
                              (isSelected ? arrowColor : IM_COL32(200, 200, 200, 255));
            draw_list->AddText(index_pos, indexColor, index_text);
        }

        // Draw arrow indicator
        if (showArrow && arrowIndex >= 0 && arrowIndex <= (int)arrayDS.data.size()) {
            float arrow_x = canvas_pos.x + arrowIndex * (box_size + spacing) + box_size * 0.5f;
            float arrow_base_y = canvas_pos.y + box_size + 30.0f;

            ImVec2 arrow_tip = ImVec2(arrow_x, canvas_pos.y + box_size + 25.0f);
            ImVec2 arrow_left = ImVec2(arrow_x - 8.0f, arrow_base_y);
            ImVec2 arrow_right = ImVec2(arrow_x + 8.0f, arrow_base_y);
            ImVec2 arrow_shaft_bottom = ImVec2(arrow_x, arrow_base_y + 15.0f);

            draw_list->AddLine(arrow_shaft_bottom, ImVec2(arrow_x, arrow_base_y), arrowColor, 3.0f);
            draw_list->AddTriangleFilled(arrow_tip, arrow_left, arrow_right, arrowColor);

            const char* label = (selectedOperation == 0) ? "Insert here" : "Delete this";
            ImVec2 label_size = ImGui::CalcTextSize(label);
            ImVec2 label_pos = ImVec2(arrow_x - label_size.x * 0.5f, arrow_shaft_bottom.y + 5.0f);
            draw_list->AddText(label_pos, arrowColor, label);
        }

        // Reserve space
        ImGui::Dummy(ImVec2((arrayDS.data.size() + 1) * (box_size + spacing), box_size + 80.0f));
    }

    /**
     * Render operation selection and controls
     */
    void renderOperationControls(ArrayStructure& arrayDS, VisualizationController& controller) {
        ImGui::Text("Select Array Operation:");
        const char* arrayOps[] = { "Insert", "Delete", "Sort", "Reset" };
        ImGui::Combo("##arrayOperation", &selectedOperation, arrayOps, IM_ARRAYSIZE(arrayOps));
        ImGui::Spacing();

        if (selectedOperation == 0) { // Insert
            ImGui::Text("Insert Operation:");
            ImGui::InputInt("Index##insert", &insertIndex);
            ImGui::InputInt("Value##insert", &insertValue);
            if (ImGui::Button("Execute Insert")) {
                if (insertIndex >= 0 && insertIndex <= (int)arrayDS.data.size()) {
                    controller.stageOperation(std::make_unique<ArrayInsert>(arrayDS, insertIndex, insertValue), &arrayDS);
                }
            }
        } else if (selectedOperation == 1) { // Delete
            ImGui::Text("Delete Operation:");
            ImGui::InputInt("Index##delete", &deleteIndex);
            if (ImGui::Button("Execute Delete")) {
                if (deleteIndex >= 0 && deleteIndex < (int)arrayDS.data.size()) {
                    controller.stageOperation(std::make_unique<ArrayDelete>(arrayDS, deleteIndex), &arrayDS);
                }
            }
        } else if (selectedOperation == 2) { // Sort
            ImGui::Text("Sort Operation:");
            ImGui::Text("Sorts the array using Bubble Sort algorithm.");
            if (ImGui::Button("Execute Sort")) {
                controller.stageOperation(std::make_unique<ArraySort>(arrayDS), &arrayDS);
            }
        } else if (selectedOperation == 3) { // Reset
            ImGui::Text("Reset Operation:");
            ImGui::Text("Resets array to [5, 2, 8, 1, 9].");
            if (ImGui::Button("Execute Reset")) {
                std::vector<int> resetValues = {5, 2, 8, 1, 9};
                controller.stageOperation(std::make_unique<ArrayInit>(resetValues), &arrayDS);
            }
        }
    }
};
