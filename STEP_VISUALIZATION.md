# Step-by-Step Visualization Feature

## Overview
Added a step-through visualization system that allows users to see each atomic operation execute one at a time, rather than executing the entire user operation at once.

## How It Works

### 1. Operation Staging
When a user clicks any operation button (Insert, Delete, Sort, Push, Pop, etc.), the operation is **staged** rather than immediately executed:
- The operation is stored as `stagedOperation`
- The target data structure is stored as `stagedDataStructure`
- The visualization state is reset (`currentAtomicStep = 0`, `isVisualizing = false`)

### 2. Visualization Control Section
A new control section appears at the top of the window showing:
- The name of the staged operation
- Total number of atomic operations in the user operation
- Current step progress (when visualizing)
- Control buttons (Start/Step/Cancel)

### 3. Start Button
When the user clicks "Start Visualization":
- Sets `isVisualizing = true`
- The button changes to "Step (Execute Next Atomic Operation)"

### 4. Step Button
Each time the user clicks "Step":
- Executes the next atomic operation: `stagedOperation->operations[currentAtomicStep]->execute(*stagedDataStructure)`
- Increments `currentAtomicStep`
- Updates the data structure state immediately (visible in the GUI)
- When all atomic operations complete:
  - Adds the complete user operation to the operation manager history
  - Resets the visualization state
  - Clears the staged operation

### 5. Cancel Button
Before starting visualization, users can click "Cancel" to discard the staged operation without executing anything.

## Implementation Details

### State Variables
```cpp
std::unique_ptr<UserOperation> stagedOperation;  // The operation being visualized
DataStructure* stagedDataStructure;               // Target data structure
size_t currentAtomicStep;                         // Current step index
bool isVisualizing;                               // Whether visualization has started
```

### Key Methods
- `stageOperation()`: Stores an operation for visualization
- `renderVisualizationControl()`: Renders the Start/Step/Cancel controls

### Modified Methods
All operation buttons now call `stageOperation()` instead of `opManager.executeOperation()`:
- Array operations: Insert, Delete, Sort, Reset
- Stack operations: Push, Pop, Clear, Initialize

## User Workflow

1. **Select an operation**: Click any operation button (e.g., "Insert Element")
2. **Review**: The operation is staged - see its name and atomic operation count
3. **Start**: Click "Start Visualization" to begin
4. **Step through**: Click "Step" repeatedly to execute each atomic operation
5. **Observe**: Watch the data structure change after each step
6. **Complete**: When all steps finish, the operation is added to history

## Example: Array Insert

For `ArrayInsert(index=2, value=99)` on array `[1, 2, 3, 4]`:
1. **Stage**: Click "Insert Element" - shows "Staged Operation: Insert Element, Total atomic operations: 3"
2. **Start**: Click "Start Visualization"
3. **Step 1**: Move element at index 3 → Visible: `[1, 2, 3, 4, _]`
4. **Step 2**: Move element at index 2 → Visible: `[1, 2, _, 3, 4]`
5. **Step 3**: Write value 99 at index 2 → Visible: `[1, 2, 99, 3, 4]`
6. **Complete**: Operation added to history, can undo/redo

## Benefits

1. **Educational**: Users can see exactly how complex operations work internally
2. **Debugging**: Helps understand operation behavior step-by-step
3. **Control**: Users control the pace of visualization
4. **Transparency**: Shows the atomic operations that compose user operations

## Architecture

The feature maintains the existing architecture:
- **Atomic operations** (WriteOp, MoveOp, PushOp, PopOp) remain the building blocks
- **User operations** (ArrayInsert, StackPush, etc.) compose atomic operations
- **Operation Manager** stores completed operations in history
- Operations are only added to history after **all** steps complete

This ensures:
- Undo/redo works on complete user operations only
- No partial operations in history
- Data structure state updates are visible immediately during stepping
