#pragma once
#include <string>
#include <memory>

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
};
