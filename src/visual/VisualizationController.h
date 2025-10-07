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
     * Get the current atomic operation to be executed (shown before execution)
     * Returns nullptr if no operation is staged or index is out of bounds
     */
    Operation* getCurrentAtomicOperation() const {
        if (!stagedOperation || !isVisualizing || currentAtomicStep >= stagedOperation->operations.size()) {
            return nullptr;
        }
        return stagedOperation->operations[currentAtomicStep].get();
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
            // Show current step as 1-indexed (step N means we're about to execute operation N)
            ImGui::Text("Current step: %zu / %zu", currentAtomicStep + 1, stagedOperation->operations.size());
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
                // Show what operation will be executed
                std::string buttonText = "Step (" + stagedOperation->operations[currentAtomicStep]->getDescription() + ")";
                if (ImGui::Button(buttonText.c_str())) {
                    // Execute the current atomic operation
                    stagedOperation->operations[currentAtomicStep]->execute(*stagedDataStructure);
                    currentAtomicStep++;

                    // If we've finished all steps, add to history
                    if (currentAtomicStep >= stagedOperation->operations.size()) {
                        // Pass true for alreadyExecuted since we've already executed through stepping
                        opManager.executeOperation(*stagedDataStructure, std::move(stagedOperation), true);
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
