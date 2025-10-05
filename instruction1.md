### **AI Agent Instruction Prompt**

---

**Prompt:**

> You are an AI coding assistant.
> Your task is to implement a **data structure visualizer** in an object-oriented language (C++17).
> The visualizer should support **multiple data structure types** (Array, Stack, Tree) and allow visualizing their operations with undo/redo capability.
> The system must be extensible so that new data structures and operations can be added easily.
>
> You must follow this **project file structure**:
>
> ```
> data-structure-visualizer/
> ├── CMakeLists.txt
> ├── src/
> │   ├── core/
> │   │   ├── DataStructure.h
> │   │   ├── Operation.h
> │   │   ├── UserOperation.h
> │   │   ├── Visualizer.h
> │   │   ├── Visualizer.cpp
> │   │   └── OperationManager.h
> │   │
> │   ├── data_structure/
> │   │   ├── ArrayStructure.h
> │   │   ├── ArrayStructure.cpp
> │   │   ├── StackStructure.h
> │   │   ├── StackStructure.cpp
> │   │   ├── TreeStructure.h
> │   │   ├── BinarySearchTree.h
> │   │   ├── AVLTree.h
> │   │   ├── HuffmanTree.h
> │   │   └── ...
> │   │
> │   ├── operation/
> │   │   ├── ArrayOps.h
> │   │   ├── StackOps.h
> │   │   ├── TreeOps.h
> │   │   └── ...
> │   │
> │   ├── visual/
> │   │   ├── GuiVisualizer.cpp
> │   │   ├── ConsoleVisualizer.cpp
> │   │   └── AnimationManager.h
> │   │
> │   └── main.cpp
> │
> └── include/
> ```
>
> ---
>
> **Design Requirements**
>
> 1. **Core Interfaces**
>
>    * `DataStructure`: abstract base for all structures, with virtual functions for initialization and access.
>    * `Operation`: abstract base for all atomic operations, with:
>
>      * `execute(DataStructure&)`
>      * `undo(DataStructure&)`
>      * `draw(Visualizer&)`
>      * `serialize()`
>      * `deserialize(const nlohmann::json&)`
>    * `UserOperation`: composite operation consisting of multiple atomic operations.
>    * `Visualizer`: interface for rendering operations.
> 2. **Atomic Operations**
>
>    * Must be grouped into single files per data structure in the `operation/` directory.
>    * Examples:
>
>      * ArrayOps.h → `WriteOp`, `MoveOp`
>      * StackOps.h → `PushOp`, `PopOp`
>      * TreeOps.h → `RotateOp`, `AttachNodeOp`, etc.
> 3. **User Operations**
>
>    * Built from atomic operations.
>    * Examples:
>
>      * ArrayOps.h → `ArrayInit`, `ArrayInsert`, `ArrayDelete`
>      * StackOps.h → `StackInit`, `StackPush`, `StackPop`
> 4. **Serialization**
>
>    * Every `Operation` must implement `serialize()` and `deserialize()` using JSON (nlohmann/json library).
> 5. **OperationManager**
>
>    * Handles storing executed operations and managing undo/redo stacks.
> 6. **Visualization**
>
>    * Implement at least one GUI visualizer (`GuiVisualizer.cpp`) and optionally a console visualizer for debugging.
>
> ---
>
> **Example: `Operation.h`**
>
> ```cpp
> #pragma once
> #include <nlohmann/json.hpp>
> class DataStructure;
> class Visualizer;
>
> class Operation {
> public:
>     virtual void execute(DataStructure&) = 0;
>     virtual void undo(DataStructure&) = 0;
>     virtual void draw(Visualizer&) = 0;
>     virtual nlohmann::json serialize() const = 0;
>     virtual void deserialize(const nlohmann::json&) = 0;
>     virtual ~Operation() = default;
> };
> ```
>
> ---
>
> **Example: `ArrayOps.h`**
>
> ```cpp
> #pragma once
> #include "../core/Operation.h"
> #include "../data_structure/ArrayStructure.h"
> #include "../core/Visualizer.h"
>
> class WriteOp : public Operation {
>     int index;
>     int oldValue;
>     int newValue;
> public:
>     WriteOp(ArrayStructure& arr, int idx, int newV);
>     WriteOp(int idx, int oldV, int newV);
>     void execute(DataStructure&) override;
>     void undo(DataStructure&) override;
>     void draw(Visualizer&) override;
>     nlohmann::json serialize() const override;
>     void deserialize(const nlohmann::json&) override;
> };
>
> class MoveOp : public Operation { /* similar */ };
>
> class ArrayInit : public UserOperation { /* similar */ };
> class ArrayInsert : public UserOperation { /* similar */ };
> class ArrayDelete : public UserOperation { /* similar */ };
> ```
>
> ---
>
> **Task for AI Agent:**
>
> * Implement the **full file structure** above with the required interfaces.
> * Implement example **ArrayOps.h** and **StackOps.h** with at least two atomic and one user operation each.
> * Implement **OperationManager** with undo/redo stack logic.
> * Implement **serialization** for operations and user operations.
> * Implement **GuiVisualizer** with example draw methods.
> * Ensure all files compile together into a working demo showing array and stack visualizations.
>
> Do not implement the history system yet — focus only on the serialization interface and the above structure.
