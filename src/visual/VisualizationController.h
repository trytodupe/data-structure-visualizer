#pragma once
#include "imgui.h"
#include "../core/OperationManager.h"
#include "../core/UserOperation.h"
#include "../core/DataStructure.h"
#include <memory>

/**
 * Controls step-by-step visualization of user operations.
 * Manages operation staging, stepping through atomic operations,
 * and rendering visualization controls (Start/Step/Cancel buttons).
 */
class VisualizationController {
private:
    // Visualization state
    std::unique_ptr<UserOperation> stagedOperation;
    DataStructure* stagedDataStructure;
    size_t currentAtomicStep;
    bool isVisualizing;

public:
    VisualizationController()
        : stagedOperation(nullptr),
          stagedDataStructure(nullptr),
          currentAtomicStep(0),
          isVisualizing(false) {}

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
     * Check if currently in visualization phase
     */
    bool isInVisualizationPhase() const {
        return isVisualizing;
    }

    /**
     * Check if an operation is staged
     */
    bool hasOperationStaged() const {
        return stagedOperation != nullptr;
    }

    /**
     * Get the current atomic operation being executed
     * Returns nullptr if no operation is being executed or index is out of bounds
     */
    Operation* getCurrentAtomicOperation() const {
        if (!stagedOperation || currentAtomicStep == 0 || currentAtomicStep > stagedOperation->operations.size()) {
            return nullptr;
        }
        return stagedOperation->operations[currentAtomicStep - 1].get();
    }

    /**
     * Get the current atomic step index
     */
    size_t getCurrentStep() const {
        return currentAtomicStep;
    }

    /**
     * Get total number of atomic operations in staged operation
     */
    size_t getTotalSteps() const {
        return stagedOperation ? stagedOperation->operations.size() : 0;
    }

    /**
     * Render the visualization control buttons (Start/Step/Cancel)
     */
    void renderControls(OperationManager& opManager) {
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
