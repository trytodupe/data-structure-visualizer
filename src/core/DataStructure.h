#pragma once
#include <string>
#include <memory>
#include "imgui.h"

/**
 * Abstract base class for all data structures.
 * Provides interface for initialization, access, and identification.
 */
class DataStructure {
public:
    virtual ~DataStructure() = default;

    /**
     * Initialize the data structure with default or specified parameters
     */
    virtual void initialize() = 0;

    /**
     * Get the type name of this data structure
     * @return String identifier for the data structure type
     */
    virtual std::string getType() const = 0;

    /**
     * Get a human-readable description of current state
     * @return String describing the current state
     */
    virtual std::string getStateDescription() const = 0;

    /**
     * Draw the current state of the data structure
     * @param startPos Starting position for drawing
     * @param boxSize Size of each element box
     * @param spacing Spacing between elements
     * @return The size of the drawn area (width, height)
     */
    virtual ImVec2 draw(ImVec2 startPos, float boxSize, float spacing) const = 0;
};
