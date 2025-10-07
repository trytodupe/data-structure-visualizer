#pragma once
#include <nlohmann/json.hpp>
#include <memory>
#include "imgui.h"

class DataStructure;
class GuiVisualizer;

/**
 * Abstract base class for all operations.
 * Represents an atomic operation that can be executed, undone, drawn, and serialized.
 */
class Operation {
public:
    virtual ~Operation() = default;

    /**
     * Execute this operation on the given data structure
     * @param ds The data structure to modify
     */
    virtual void execute(DataStructure& ds) = 0;

    /**
     * Undo this operation on the given data structure
     * @param ds The data structure to restore
     */
    virtual void undo(DataStructure& ds) = 0;

    /**
     * Draw an overlay on top of the data structure visualization
     * This highlights the elements being modified by this operation
     * @param ds The data structure being operated on
     * @param startPos Starting position where the data structure is drawn
     * @param boxSize Size of each element box
     * @param spacing Spacing between elements
     */
    virtual void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const = 0;

    /**
     * Serialize this operation to JSON
     * @return JSON representation of this operation
     */
    virtual nlohmann::json serialize() const = 0;

    /**
     * Deserialize this operation from JSON
     * @param j JSON object containing operation data
     */
    virtual void deserialize(const nlohmann::json& j) = 0;

    /**
     * Get a description of this operation
     * @return String describing what this operation does
     */
    virtual std::string getDescription() const = 0;

    /**
     * Clone this operation
     * @return A deep copy of this operation
     */
    virtual std::unique_ptr<Operation> clone() const = 0;
};
