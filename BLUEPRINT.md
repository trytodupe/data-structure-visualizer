# Data Structure Visualizer - Project Blueprint

## Project Overview

A real-time data structure visualizer built with C++ and ImGui that allows users to:
- Visualize linear data structures (arrays, linked lists, stacks, queues)
- Visualize tree data structures (binary trees, BST, AVL, etc.)
- Animate operations (insert, delete, search, traversal)
- Save/load data structure states to/from files
- Pure backend data structures with separate visualization layer

## Architecture Overview

### Core Principles
1. **Separation of Concerns**: Backend data structures are pure, frontend handles all visualization
2. **Operation Recording**: All operations are recorded for animation and state management
3. **Serialization**: Full state persistence with JSON format
4. **Extensibility**: Easy to add new data structures and visualization modes

## Project Structure

```
src/
├── main.cpp                    # Application entry point
├── core/                       # Pure data structures (backend)
│   ├── DataStructure.h         # Base interface for all data structures
│   ├── Operation.h             # Operation recording system with data structure-specific types
│   ├── OperationTypes.h        # Operation type definitions for each data structure
│   ├── Serializer.h            # JSON serialization interface
│   ├── linear/                 # Linear data structures
│   │   ├── DynamicArray.h/.cpp # Array with init, insert, delete operations
│   │   ├── LinkedList.h/.cpp   # Linked list with init, insert, delete operations
│   │   └── Stack.h/.cpp        # Stack with init, push, pop operations
│   └── tree/                   # Tree data structures
│       ├── BinaryTree.h/.cpp   # Basic binary tree with init operation only
│       ├── BST.h/.cpp          # Binary search tree with init, search, insert, delete
│       ├── AVLTree.h/.cpp      # AVL tree with init operation (shows auto-balancing)
│       └── HuffmanTree.h/.cpp  # Huffman tree with init operation (encoding tree)
├── llm/                        # LLM integration for natural language processing
│   ├── LLMParser.h/.cpp        # API-based LLM parser (OpenAI, Anthropic, etc.)
│   ├── CommandExecutor.h/.cpp  # Execute parsed natural language commands
│   └── HTTPClient.h/.cpp       # HTTP client for LLM API communication
├── visualization/              # Frontend visualization layer
│   ├── Renderer.h              # Base renderer interface
│   ├── LinearRenderer.h/.cpp   # Renderer for arrays, lists, stacks
│   ├── TreeRenderer.h/.cpp     # Renderer for all tree structures
│   ├── AnimationSystem.h/.cpp  # Operation animation and playback system
│   ├── LayoutManager.h/.cpp    # Auto-positioning algorithms for nodes
│   └── Theme.h/.cpp            # Visual styling and color schemes
├── ui/                         # ImGui application layer
│   ├── Application.h/.cpp      # Main application window and state management
│   ├── ControlPanel.h/.cpp     # Structure selection and operation controls
│   ├── VisualizationPanel.h/.cpp # Main visualization canvas with zoom/pan
│   ├── StatusPanel.h/.cpp      # Status display and operation history
│   ├── LLMPanel.h/.cpp         # Natural language input and suggestions
│   └── OperationPanel.h/.cpp   # Dynamic operation buttons based on selected structure
└── utils/                      # Utility classes
    ├── FileIO.h/.cpp           # JSON file save/load operations
    ├── Math.h/.cpp             # Mathematical utilities for positioning
    └── Config.h/.cpp           # Application configuration and settings
```

### File Organization Details

**Core Data Structures (`src/core/`):**
- **OperationTypes.h**: Centralized operation type definitions
  ```cpp
  namespace LinearOps { enum class Type { INIT, INSERT, DELETE }; }
  namespace StackOps { enum class Type { INIT, PUSH, POP }; }
  namespace BSTOps { enum class Type { INIT, SEARCH, INSERT, DELETE }; }
  // ... etc for each data structure
  ```

**LLM Integration (`src/llm/`):**
- **HTTPClient**: Handles secure HTTPS communication with LLM APIs
- **LLMParser**: Converts natural language to structured JSON commands
- **CommandExecutor**: Validates and executes parsed commands on data structures

**UI Components (`src/ui/`):**
- **OperationPanel**: Dynamically shows only supported operations for selected structure
- **LLMPanel**: Natural language input with real-time parsing feedback
- **Enhanced ControlPanel**: Structure selection with operation-specific controls

**Configuration Files:**
```
config/
├── settings.json              # Application settings and preferences
├── themes.json               # Visual theme definitions
└── llm_config.json           # LLM API configuration (endpoints, models)

models/                       # Optional: for future local LLM support
└── README.md                 # Instructions for local model setup

examples/                     # Sample data structure files
├── bst_example.json         # Example BST with operations
├── stack_demo.json          # Stack operation sequence
└── huffman_example.json     # Huffman tree encoding example
```

## Core Components Detail

### 1. Backend Data Structures (`src/core/`)

#### Base Interface (`DataStructure.h`)
```cpp
class DataStructure {
public:
    virtual ~DataStructure() = default;
    virtual std::string getType() const = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual void clear() = 0;
    virtual nlohmann::json serialize() const = 0;
    virtual void deserialize(const nlohmann::json& j) = 0;
    virtual std::vector<Operation> getOperationHistory() const = 0;
    virtual void clearOperationHistory() = 0;

    // New methods for operation support
    virtual std::vector<int> getSupportedOperations() const = 0;  // Returns supported operation types
    virtual bool supportsOperation(int operationType) const = 0;  // Check if operation is supported
    virtual std::string getOperationName(int operationType) const = 0;  // Get human-readable name

    // Initialize with data set (required by all structures)
    virtual bool initialize(const std::vector<int>& data) = 0;
};

// Example implementation for different data structures
class DynamicArray : public DataStructure {
public:
    std::vector<int> getSupportedOperations() const override {
        return {
            static_cast<int>(LinearOps::Type::INIT),
            static_cast<int>(LinearOps::Type::INSERT),
            static_cast<int>(LinearOps::Type::DELETE)
        };
    }

    bool supportsOperation(int operationType) const override {
        auto supported = getSupportedOperations();
        return std::find(supported.begin(), supported.end(), operationType) != supported.end();
    }

    std::string getOperationName(int operationType) const override {
        switch(static_cast<LinearOps::Type>(operationType)) {
            case LinearOps::Type::INIT: return "Initialize";
            case LinearOps::Type::INSERT: return "Insert";
            case LinearOps::Type::DELETE: return "Delete";
            default: return "Unknown";
        }
    }

    bool initialize(const std::vector<int>& data) override {
        // Initialize array with given data
        elements = data;
        recordOperation(LinearOps::Type::INIT, {}, data, "Initialize array with data");
        return true;
    }
};

class Stack : public DataStructure {
public:
    std::vector<int> getSupportedOperations() const override {
        return {
            static_cast<int>(StackOps::Type::INIT),
            static_cast<int>(StackOps::Type::PUSH),
            static_cast<int>(StackOps::Type::POP)
        };
    }

    std::string getOperationName(int operationType) const override {
        switch(static_cast<StackOps::Type>(operationType)) {
            case StackOps::Type::INIT: return "Initialize";
            case StackOps::Type::PUSH: return "Push";
            case StackOps::Type::POP: return "Pop";
            default: return "Unknown";
        }
    }
};
```

#### Operation Recording (`Operation.h`)
```cpp
// Base operation types - extensible design
enum class BaseOperationType {
    INIT,           // Initialize with data set
    INSERT,         // Generic insertion
    DELETE,         // Generic deletion
    SEARCH,         // Search operation
    CUSTOM          // Custom operation (defined by subclass)
};

// Data structure specific operations
namespace LinearOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        INSERT = static_cast<int>(BaseOperationType::INSERT),
        DELETE = static_cast<int>(BaseOperationType::DELETE),
        // List-specific operations can be added here
    };
}

namespace StackOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        PUSH = static_cast<int>(BaseOperationType::INSERT),  // Push maps to insert
        POP = static_cast<int>(BaseOperationType::DELETE),   // Pop maps to delete
    };
}

namespace TreeOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        // Basic trees only support initialization
    };
}

namespace BSTOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        SEARCH = static_cast<int>(BaseOperationType::SEARCH),
        INSERT = static_cast<int>(BaseOperationType::INSERT),
        DELETE = static_cast<int>(BaseOperationType::DELETE),
    };
}

namespace AVLOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        // AVL trees primarily show initialization and auto-balancing
    };
}

namespace HuffmanOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        // Huffman trees show encoding tree construction
    };
}

struct Operation {
    BaseOperationType baseType;      // Base operation type
    int specificType;                // Data structure specific type
    std::string dataStructureType;   // Which data structure this applies to
    std::vector<int> indices;        // Affected positions
    std::vector<int> values;         // Values involved
    std::string description;         // Human-readable description
    std::chrono::timestamp time;     // When operation occurred
    bool success;                    // Whether operation succeeded

    // Helper methods for type checking
    template<typename SpecificType>
    bool isType(SpecificType type) const {
        return specificType == static_cast<int>(type);
    }
};
```

#### Data Structure Implementations

**Linear Data Structures:**
- **DynamicArray/LinkedList**:
  - **Operations**: `INIT` (create with data set), `INSERT` (add element), `DELETE` (remove element)
  - **Focus**: Basic linear structure operations without sorting algorithms
  - **Visualization**: Show array/list growth, element positioning, insertion/deletion animations

**Stack Structures:**
- **Stack**:
  - **Operations**: `INIT` (create with initial items), `PUSH` (add to top), `POP` (remove from top)
  - **Focus**: LIFO behavior demonstration
  - **Visualization**: Vertical stack with top highlighting, push/pop animations

**Tree Structures:**
- **BinaryTree**:
  - **Operations**: `INIT` (create tree with given structure)
  - **Focus**: Tree structure visualization and traversal demonstration
  - **Visualization**: Hierarchical layout, node connections, traversal highlighting

- **Binary Search Tree (BST)**:
  - **Operations**: `INIT` (create with data set), `SEARCH` (find element), `INSERT` (add element), `DELETE` (remove element)
  - **Focus**: Search tree properties and operations
  - **Visualization**: Search path highlighting, insertion positioning, deletion cases

- **AVL Tree**:
  - **Operations**: `INIT` (create balanced tree with data set)
  - **Focus**: Self-balancing demonstration during construction
  - **Visualization**: Balance factor display, rotation animations during initialization

- **Huffman Tree**:
  - **Operations**: `INIT` (build encoding tree from frequency data)
  - **Focus**: Encoding tree construction process
  - **Visualization**: Bottom-up tree building, frequency merging, code assignment

#### Extensible Operation System

**Adding New Data Structures:**
To add a new data structure with custom operations:

1. **Define Operation Namespace:**
```cpp
namespace GraphOps {
    enum class Type {
        INIT = static_cast<int>(BaseOperationType::INIT),
        ADD_VERTEX = static_cast<int>(BaseOperationType::CUSTOM) + 1,
        ADD_EDGE = static_cast<int>(BaseOperationType::CUSTOM) + 2,
        DFS = static_cast<int>(BaseOperationType::CUSTOM) + 3,
        BFS = static_cast<int>(BaseOperationType::CUSTOM) + 4,
    };
}
```

2. **Implement DataStructure Interface:**
```cpp
class Graph : public DataStructure {
public:
    std::vector<int> getSupportedOperations() const override {
        return {
            static_cast<int>(GraphOps::Type::INIT),
            static_cast<int>(GraphOps::Type::ADD_VERTEX),
            static_cast<int>(GraphOps::Type::ADD_EDGE),
            static_cast<int>(GraphOps::Type::DFS),
            static_cast<int>(GraphOps::Type::BFS)
        };
    }

    std::string getOperationName(int operationType) const override {
        switch(static_cast<GraphOps::Type>(operationType)) {
            case GraphOps::Type::INIT: return "Initialize";
            case GraphOps::Type::ADD_VERTEX: return "Add Vertex";
            case GraphOps::Type::ADD_EDGE: return "Add Edge";
            case GraphOps::Type::DFS: return "Depth-First Search";
            case GraphOps::Type::BFS: return "Breadth-First Search";
            default: return "Unknown";
        }
    }
};
```

3. **Update LLM Grammar:** Add the new structure and operations to the command parser template.

### 2. Visualization Layer (`src/visualization/`)

#### Base Renderer (`Renderer.h`)
```cpp
class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void render(const DataStructure* ds, const RenderContext& ctx) = 0;
    virtual void renderOperation(const Operation& op, float progress) = 0;
    virtual void setTheme(const Theme& theme) = 0;
    virtual ImVec2 getRequiredSize(const DataStructure* ds) const = 0;
};
```

#### Animation System (`AnimationSystem.h`)
- **Operation Playback**: Step through recorded operations
- **Speed Control**: Adjustable animation speed (0.1x to 5x)
- **Pause/Resume**: Full control over animation state
- **Step Mode**: Manual step-through for detailed analysis
- **Timeline Scrubbing**: Jump to any point in operation history

#### Rendering Features
- **Node Visualization**: Colored boxes/circles with values
- **Connection Lines**: Arrows and lines showing relationships
- **Highlighting**: Active elements during operations
- **Fade Effects**: Smooth transitions for insert/delete
- **Layout Algorithms**: Auto-positioning for optimal viewing

### 3. User Interface (`src/ui/`)

#### Main Application (`Application.h`)
- **Window Management**: ImGui window lifecycle
- **State Management**: Current data structure and mode
- **Event Handling**: User input processing
- **File Operations**: Save/load integration

#### Control Panel (`ControlPanel.h`)
- **Data Structure Selection**: Dropdown for structure type
- **Operation Buttons**: Insert, Delete, Search, Clear
- **Value Input**: Text field for operation values
- **Animation Controls**: Play, pause, step, speed adjustment
- **File Operations**: Save, load, export buttons

#### Visualization Panel (`VisualizationPanel.h`)
- **Render Area**: Main visualization canvas
- **Zoom/Pan**: Interactive view controls
- **Grid Overlay**: Optional alignment grid
- **Minimap**: Overview for large structures

### 4. Serialization System

#### JSON Format Structure
```json
{
    "metadata": {
        "type": "DynamicArray",
        "version": "1.0",
        "created": "2025-09-26T10:30:00Z"
    },
    "data": {
        "elements": [1, 2, 3, 4, 5],
        "capacity": 8,
        "size": 5
    },
    "operations": [
        {
            "type": "INSERT",
            "index": 0,
            "value": 1,
            "description": "Insert 1 at position 0",
            "timestamp": "2025-09-26T10:30:01Z",
            "success": true
        }
    ],
    "visualization": {
        "theme": "default",
        "layout": "horizontal",
        "spacing": 50
    }
}
```

## Implementation Phases

### Phase 1: Core Foundation
1. **Project Setup**: Build system, dependencies, basic ImGui window
2. **Base Classes**: DataStructure interface, Operation system, basic serialization
3. **Simple Array**: Implement DynamicArray with basic operations
4. **Basic Renderer**: LinearRenderer for array visualization
5. **Minimal UI**: Basic control panel and visualization area

### Phase 2: Linear Structures
1. **LinkedList**: Complete implementation with node visualization
2. **Stack/Queue**: LIFO/FIFO structures with specialized animations
3. **Enhanced Animations**: Smooth transitions, highlighting, operation traces
4. **File I/O**: Save/load functionality for all linear structures
5. **UI Polish**: Better controls, status display, error handling

### Phase 3: Tree Structures
1. **BinaryTree**: Basic binary tree with level-order layout
2. **TreeRenderer**: Specialized renderer for hierarchical structures
3. **BST Implementation**: Search tree with operation animations
4. **Advanced Layouts**: Multiple tree layout algorithms
5. **Performance**: Optimization for large tree visualization

### Phase 4: Advanced Features
1. **AVL Trees**: Self-balancing with rotation animations
2. **Algorithm Visualization**: Traversal algorithms (inorder, preorder, postorder)
3. **Comparison Mode**: Side-by-side structure comparison
4. **Export Features**: Image/video export of animations
5. **Plugin System**: Framework for adding custom structures

### Phase 5: LLM Integration
1. **API Setup**: HTTP client for LLM API communication (OpenAI, Anthropic, etc.)
2. **Command Parser**: Natural language to JSON conversion system
3. **Command Executor**: System to execute parsed natural language commands
4. **Enhanced UI**: Natural language input panel with loading states
5. **Error Handling**: Robust error handling and user feedback

## Technical Requirements

### Dependencies
- **ImGui**: GUI framework (already included)
- **GLFW**: Window and input handling (already included)
- **OpenGL**: Graphics rendering (already included)
- **nlohmann/json**: JSON serialization (already included)
- **C++17**: Modern C++ features (range-based loops, auto, etc.)
- **libcurl** (optional): HTTP client for LLM API integration

### Performance Considerations
- **Lazy Rendering**: Only render visible elements
- **LOD System**: Level-of-detail for large structures
- **Memory Management**: Efficient node allocation/deallocation
- **Update Throttling**: Limit refresh rate for smooth animations

### Cross-Platform Support
- **Build System**: Makefile with platform detection
- **Graphics**: OpenGL 3.3+ compatibility
- **File Paths**: Cross-platform path handling
- **Fonts**: Embedded font support for consistency

## User Experience Design

### Workflow
1. **Select Structure**: Choose from dropdown menu
2. **Add Elements**: Insert values using input field
3. **Perform Operations**: Click operation buttons or use keyboard shortcuts
4. **Watch Animation**: Observe real-time visualization
5. **Save State**: Export current structure to file
6. **Load State**: Import previously saved structure

### Visual Design
- **Color Coding**: Different colors for different element states
- **Smooth Animations**: 60 FPS interpolated transitions
- **Responsive Layout**: Adapts to window resizing
- **Accessibility**: High contrast mode, keyboard navigation
- **Themes**: Multiple visual themes (dark, light, high contrast)

### Error Handling
- **Input Validation**: Check bounds, types, constraints
- **Graceful Degradation**: Fallback for unsupported operations
- **User Feedback**: Clear error messages and status updates
- **Recovery**: Undo/redo functionality for operations

## LLM Integration Feature

### Natural Language Processing
A powerful feature that allows users to describe data structures and operations in natural language, automatically converting them to executable operations.

#### Component: LLM Parser (`src/llm/`)
```cpp
class LLMParser {
public:
    struct ParsedCommand {
        std::string dataStructureType;     // "binary_search_tree", "array", etc.
        std::vector<int> initialData;      // Initial elements
        std::vector<Operation> operations; // Sequence of operations to perform
        std::string algorithm;             // Algorithm to apply (optional)
        bool success;                      // Whether parsing succeeded
        std::string errorMessage;          // Error details if failed
    };

    ParsedCommand parseNaturalLanguage(const std::string& input);
    bool validateCommand(const ParsedCommand& cmd);
    std::string generateHelpText() const;
};
```

#### Integration Architecture
```
User Input (Natural Language)
    ↓
HTTP Request to LLM API
    ↓
LLM Response (JSON)
    ↓
Command Executor
    ↓
Data Structure + Operations
    ↓
Visualization System
```

#### Example Use Cases

**Data Structure Creation:**
- *"Create a binary search tree with elements [5,3,7,2,4,6,8]"*
- *"Make an array with values 1 through 10"*
- *"Generate a stack with items [a,b,c,d]"*

**Operation Sequences:**
- *"Insert 15 into the BST, then delete 3"*
- *"Push 5 elements onto the stack, then pop 2"*
- *"Sort the array using bubble sort algorithm"*

**Algorithm Demonstrations:**
- *"Show inorder traversal of the binary tree"*
- *"Demonstrate quicksort on array [3,1,4,1,5,9]"*
- *"Find element 7 in the BST using binary search"*

#### LLM Integration Implementation

**Simple API-Based Approach**
```cpp
// Using external API (OpenAI, Anthropic, etc.)
class LLMParser {
private:
    std::string apiKey;
    std::string endpoint;
    std::string systemPrompt;

public:
    struct ParsedCommand {
        std::string dataStructureType;     // "binary_search_tree", "array", etc.
        std::vector<int> initialData;      // Initial elements
        std::vector<Operation> operations; // Sequence of operations to perform
        std::string algorithm;             // Algorithm to apply (optional)
        bool success;                      // Whether parsing succeeded
        std::string errorMessage;          // Error details if failed
    };

    bool initialize(const std::string& apiKey, const std::string& endpoint);
    ParsedCommand parseNaturalLanguage(const std::string& input);
    bool validateCommand(const ParsedCommand& cmd);
    std::string generateHelpText() const;
};
```

#### Command Grammar Definition

**Structured Prompt Template:**
```
You are a data structure command parser. Convert natural language to JSON commands.

Available data structures and their supported operations:

LINEAR STRUCTURES:
- array, linked_list: init, insert, delete
- stack: init, push, pop

TREE STRUCTURES:
- binary_tree: init
- bst: init, search, insert, delete
- avl_tree: init
- huffman_tree: init

Note: "init" means creating the structure with an initial set of data, not empty.

Input: "{user_input}"

Output JSON format:
{
    "data_structure": "bst",
    "operations": [
        {
            "type": "init",
            "data": [5, 3, 7, 2, 4],
            "description": "Initialize BST with elements [5,3,7,2,4]"
        },
        {
            "type": "search",
            "value": 7,
            "description": "Search for element 7"
        },
        {
            "type": "delete",
            "value": 3,
            "description": "Delete element 3"
        }
    ],
    "visualization_hints": {
        "highlight_path": true,
        "animation_speed": "normal",
        "show_step_by_step": true
    }
}

Examples:
- "Create a BST with [5,3,7,2,4]" → init operation with data
- "Make a stack with items [1,2,3], then push 4" → init + push operations
- "Build an AVL tree from [10,5,15,3,7]" → init operation (auto-balancing shown)
```

#### UI Integration

**Enhanced Control Panel (`ControlPanel.h`)**
```cpp
class ControlPanel {
private:
    std::unique_ptr<LLMParser> llmParser;
    char naturalLanguageInput[512];
    bool showLLMPanel;
    std::string lastParseResult;

public:
    void renderLLMPanel();
    void processNaturalLanguageCommand();
    void showSuggestedCommands();
};
```

**New UI Elements:**
- **Command Input Field**: Large text area for natural language input
- **Parse Button**: Convert natural language to operations
- **Suggestions Panel**: Show example commands and help text
- **Preview Panel**: Display parsed operations before execution
- **Command History**: Keep track of successful commands

#### Implementation Phases

**Phase 1: Basic API Integration**
- Set up HTTP client for LLM API communication
- Implement basic natural language to JSON conversion
- Integration with existing operation system

**Phase 2: Enhanced Features**
- Command validation and error handling
- Multi-step operation sequences
- Command suggestions and help text

#### Error Handling & User Experience

**API Error Handling:**
- Network timeout handling with retry logic
- Clear error messages for API failures
- Graceful degradation when LLM service is unavailable

**User Feedback:**
- Loading indicators during API calls
- Real-time parsing preview
- Clear success/error status messages

#### Security & Privacy Considerations

**API Integration:**
- Secure API key storage and management
- HTTPS-only communication
- Optional feature with clear privacy notice
- User controls for enabling/disabling LLM features

## Future Extensions

### Additional Data Structures
- **Hash Tables**: With collision resolution visualization
- **Graphs**: Adjacency list/matrix with traversal algorithms
- **Heaps**: Binary heap with heapify animations
- **Tries**: Prefix trees for string operations

### Advanced Features
- **Multi-threading**: Parallel algorithm visualization
- **Network Mode**: Collaborative visualization sessions
- **Educational Mode**: Step-by-step tutorials and explanations
- **Performance Metrics**: Big O analysis and timing comparisons
- **Voice Commands**: Speech-to-text integration for hands-free operation

### Integration Possibilities
- **IDE Plugin**: Integration with popular IDEs
- **Web Version**: WebAssembly port for browser use
- **Mobile Apps**: Touch-friendly mobile versions
- **API Server**: RESTful API for external integrations

## Success Metrics

### Functionality
- All data structures support CRUD operations
- Smooth animations at 60 FPS
- Save/load works reliably
- No crashes during normal operation

### Usability
- Intuitive interface requiring no documentation
- Fast response to user input (<100ms)
- Clear visual feedback for all operations
- Accessible to beginners and experts

### Performance
- Handle structures with 1000+ elements
- Memory usage under 100MB for typical use
- Startup time under 3 seconds
- File operations complete in under 1 second

This blueprint provides a comprehensive roadmap for building a robust, extensible data structure visualizer that meets all your requirements while maintaining clean architecture and excellent user experience.
