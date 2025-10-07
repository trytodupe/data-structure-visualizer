#pragma once
#include "../core/Operation.h"
#include "../core/UserOperation.h"
#include "../data_structure/BinaryTreeStructure.h"
#include <memory>
#include <vector>

// ============================================================================
// ATOMIC OPERATIONS
// ============================================================================

/**
 * Atomic operation: Set root of the tree
 */
class SetRootOp : public Operation {
private:
    int nodeId;
    int oldRootId;
    bool wasSet;

public:
    SetRootOp(int id)
        : nodeId(id), oldRootId(-1), wasSet(false) {}

    void execute(DataStructure& ds) override {
        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* node = tree.getNodeById(nodeId);

        if (!node) return;

        // Save old root
        oldRootId = tree.root ? tree.root->id : -1;

        // Set new root
        tree.root = node;

        // Remove from temp slot if present
        tree.removeFromTempSlot(node);

        wasSet = true;
    }

    void undo(DataStructure& ds) override {
        if (!wasSet) return;

        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* node = tree.getNodeById(nodeId);

        if (node) {
            // Put node back in temp slot
            tree.addToTempSlot(node);
        }

        // Restore old root
        tree.root = tree.getNodeById(oldRootId);
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        const BinaryTreeStructure& tree = dynamic_cast<const BinaryTreeStructure&>(ds);
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Highlight the temp slot -> root transition
        if (tree.tempSlot && tree.tempSlot->id == nodeId) {
            float x = startPos.x;
            float tempSlotY = startPos.y + 400.0f;

            // Draw arrow from temp slot to root position
            ImVec2 arrowStart(x + boxSize * 0.5f, tempSlotY);
            ImVec2 arrowEnd(startPos.x + 400.0f, startPos.y + 50.0f);
            drawList->AddLine(arrowStart, arrowEnd, IM_COL32(0, 255, 0, 255), 3.0f);

            // Draw text
            drawList->AddText(ImVec2(startPos.x + 200.0f, startPos.y + 200.0f),
                             IM_COL32(0, 255, 0, 255), "Set as Root");
        }
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "SetRootOp";
        j["nodeId"] = nodeId;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        nodeId = j["nodeId"];
    }

    std::string getDescription() const override {
        return "Set node " + std::to_string(nodeId) + " as root";
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<SetRootOp>(nodeId);
        op->oldRootId = oldRootId;
        op->wasSet = wasSet;
        return op;
    }
};

/**
 * Atomic operation: Create a new node
 */
class CreateNodeOp : public Operation {
private:
    int value;
    int nodeId;
    bool wasCreated;

public:
    CreateNodeOp(int val, int id)
        : value(val), nodeId(id), wasCreated(false) {}

    void execute(DataStructure& ds) override {
        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* node = new TreeNode(value, nodeId);
        tree.registerNode(node);
        tree.addToTempSlot(node);
        wasCreated = true;
    }

    void undo(DataStructure& ds) override {
        if (!wasCreated) return;

        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* node = tree.getNodeById(nodeId);
        if (node) {
            tree.removeFromTempSlot(node);
            tree.unregisterNode(node);
            delete node;
        }
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        const BinaryTreeStructure& tree = dynamic_cast<const BinaryTreeStructure&>(ds);
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Highlight the temp slot
        TreeNode* node = tree.getNodeById(nodeId);
        if (!node) return;

        if (tree.tempSlot == node) {
            float x = startPos.x;
            float tempSlotY = startPos.y + 400.0f;  // Approximate position

            ImVec2 boxMin(x - 5.0f, tempSlotY - 5.0f);
            ImVec2 boxMax(x + boxSize + 5.0f, tempSlotY + boxSize + 5.0f);
            drawList->AddRect(boxMin, boxMax, IM_COL32(255, 140, 0, 255), 0.0f, 0, 4.0f);
        }
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "CreateNodeOp";
        j["value"] = value;
        j["nodeId"] = nodeId;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        value = j["value"];
        nodeId = j["nodeId"];
    }

    std::string getDescription() const override {
        return "Create node " + std::to_string(nodeId) + " with value " + std::to_string(value);
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<CreateNodeOp>(value, nodeId);
        op->wasCreated = wasCreated;
        return op;
    }

    int getNodeId() const { return nodeId; }
};

/**
 * Atomic operation: Delete a node
 */
class DeleteNodeOp : public Operation {
private:
    int nodeId;
    int savedValue;
    bool wasDeleted;

public:
    DeleteNodeOp(int id)
        : nodeId(id), savedValue(0), wasDeleted(false) {}

    void execute(DataStructure& ds) override {
        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* node = tree.getNodeById(nodeId);
        if (!node) return;

        savedValue = node->value;
        tree.removeFromTempSlot(node);
        tree.unregisterNode(node);
        delete node;
        wasDeleted = true;
    }

    void undo(DataStructure& ds) override {
        if (!wasDeleted) return;

        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* node = new TreeNode(savedValue, nodeId);
        tree.registerNode(node);
        tree.addToTempSlot(node);
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        const BinaryTreeStructure& tree = dynamic_cast<const BinaryTreeStructure&>(ds);
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Highlight the node to be deleted (in temp slot or tree)
        TreeNode* node = tree.getNodeById(nodeId);
        if (!node) return;

        if (tree.tempSlot == node) {
            float x = startPos.x;
            float tempSlotY = startPos.y + 400.0f;

            ImVec2 boxMin(x - 5.0f, tempSlotY - 5.0f);
            ImVec2 boxMax(x + boxSize + 5.0f, tempSlotY + boxSize + 5.0f);
            drawList->AddRect(boxMin, boxMax, IM_COL32(255, 0, 0, 255), 0.0f, 0, 4.0f);
        }
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "DeleteNodeOp";
        j["nodeId"] = nodeId;
        j["savedValue"] = savedValue;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        nodeId = j["nodeId"];
        savedValue = j["savedValue"];
    }

    std::string getDescription() const override {
        return "Delete node " + std::to_string(nodeId);
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<DeleteNodeOp>(nodeId);
        op->savedValue = savedValue;
        op->wasDeleted = wasDeleted;
        return op;
    }
};

/**
 * Atomic operation: Connect child to parent
 */
class ConnectOp : public Operation {
private:
    int parentId;
    int childId;
    bool isLeftChild;  // true = left, false = right
    int oldChildId;    // For undo
    bool wasConnected;

public:
    ConnectOp(int parentNodeId, int childNodeId, bool left)
        : parentId(parentNodeId), childId(childNodeId), isLeftChild(left),
          oldChildId(-1), wasConnected(false) {}

    void execute(DataStructure& ds) override {
        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* parent = tree.getNodeById(parentId);
        TreeNode* child = tree.getNodeById(childId);

        if (!parent || !child) return;

        // Save old child for undo
        TreeNode* oldChild = isLeftChild ? parent->left : parent->right;
        oldChildId = oldChild ? oldChild->id : -1;

        // Set new connection
        if (isLeftChild) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        child->parent = parent;

        // Remove child from temp slots if present
        tree.removeFromTempSlot(child);

        wasConnected = true;
    }

    void undo(DataStructure& ds) override {
        if (!wasConnected) return;

        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* parent = tree.getNodeById(parentId);
        TreeNode* child = tree.getNodeById(childId);

        if (!parent || !child) return;

        // Restore old connection
        TreeNode* oldChild = tree.getNodeById(oldChildId);
        if (isLeftChild) {
            parent->left = oldChild;
        } else {
            parent->right = oldChild;
        }
        if (oldChild) {
            oldChild->parent = parent;
        }

        // Put child back in temp slots
        child->parent = nullptr;
        tree.addToTempSlot(child);
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        const BinaryTreeStructure& tree = dynamic_cast<const BinaryTreeStructure&>(ds);
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        TreeNode* parent = tree.getNodeById(parentId);
        TreeNode* child = tree.getNodeById(childId);

        if (!parent || !child) return;

        // Draw animated connection line (orange)
        // Note: This is simplified; actual positions would need to be calculated based on tree layout
        drawList->AddText(ImVec2(startPos.x, startPos.y + 250.0f),
                         IM_COL32(255, 140, 0, 255),
                         ("Connect node " + std::to_string(childId) + " to " +
                          std::to_string(parentId) + " (" +
                          (isLeftChild ? "left" : "right") + ")").c_str());
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "ConnectOp";
        j["parentId"] = parentId;
        j["childId"] = childId;
        j["isLeftChild"] = isLeftChild;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        parentId = j["parentId"];
        childId = j["childId"];
        isLeftChild = j["isLeftChild"];
    }

    std::string getDescription() const override {
        return "Connect node " + std::to_string(childId) + " to " +
               std::to_string(parentId) + (isLeftChild ? " (left)" : " (right)");
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<ConnectOp>(parentId, childId, isLeftChild);
        op->oldChildId = oldChildId;
        op->wasConnected = wasConnected;
        return op;
    }
};

/**
 * Atomic operation: Disconnect child from parent
 */
class DisconnectOp : public Operation {
private:
    int parentId;
    int childId;
    bool isLeftChild;
    bool wasDisconnected;

public:
    DisconnectOp(int parentNodeId, int childNodeId, bool left)
        : parentId(parentNodeId), childId(childNodeId), isLeftChild(left),
          wasDisconnected(false) {}

    void execute(DataStructure& ds) override {
        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* parent = tree.getNodeById(parentId);
        TreeNode* child = tree.getNodeById(childId);

        if (!parent || !child) return;

        // Remove connection
        if (isLeftChild) {
            parent->left = nullptr;
        } else {
            parent->right = nullptr;
        }
        child->parent = nullptr;

        // Add child to temp slots
        tree.addToTempSlot(child);

        wasDisconnected = true;
    }

    void undo(DataStructure& ds) override {
        if (!wasDisconnected) return;

        BinaryTreeStructure& tree = dynamic_cast<BinaryTreeStructure&>(ds);
        TreeNode* parent = tree.getNodeById(parentId);
        TreeNode* child = tree.getNodeById(childId);

        if (!parent || !child) return;

        // Restore connection
        if (isLeftChild) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        child->parent = parent;

        // Remove from temp slots
        tree.removeFromTempSlot(child);
    }

    void drawOverlay(const DataStructure& ds, ImVec2 startPos, float boxSize, float spacing) const override {
        const BinaryTreeStructure& tree = dynamic_cast<const BinaryTreeStructure&>(ds);
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        TreeNode* parent = tree.getNodeById(parentId);
        TreeNode* child = tree.getNodeById(childId);

        if (!parent || !child) return;

        // Draw disconnect indicator
        drawList->AddText(ImVec2(startPos.x, startPos.y + 250.0f),
                         IM_COL32(255, 0, 0, 255),
                         ("Disconnect node " + std::to_string(childId) + " from " +
                          std::to_string(parentId) + " (" +
                          (isLeftChild ? "left" : "right") + ")").c_str());
    }

    nlohmann::json serialize() const override {
        nlohmann::json j;
        j["type"] = "DisconnectOp";
        j["parentId"] = parentId;
        j["childId"] = childId;
        j["isLeftChild"] = isLeftChild;
        return j;
    }

    void deserialize(const nlohmann::json& j) override {
        parentId = j["parentId"];
        childId = j["childId"];
        isLeftChild = j["isLeftChild"];
    }

    std::string getDescription() const override {
        return "Disconnect node " + std::to_string(childId) + " from " +
               std::to_string(parentId) + (isLeftChild ? " (left)" : " (right)");
    }

    std::unique_ptr<Operation> clone() const override {
        auto op = std::make_unique<DisconnectOp>(parentId, childId, isLeftChild);
        op->wasDisconnected = wasDisconnected;
        return op;
    }
};

// ============================================================================
// USER OPERATIONS (Composite)
// ============================================================================

/**
 * User operation: Initialize binary tree with values
 * Creates nodes and connects them immediately (one at a time) in level-order
 */
class BinaryTreeInit : public UserOperation {
public:
    BinaryTreeInit(BinaryTreeStructure& tree, const std::vector<int>& values)
        : UserOperation("BinaryTreeInit", "Initialize binary tree with values") {
        // Clear the tree first (not tracked in operations)
        tree.clear();

        if (values.empty()) return;

        // Build tree in level-order: create node, connect it, then create next
        std::vector<int> nodeIds;

        // Create and set root
        int rootNodeId = tree.allocateNodeId();
        operations.push_back(std::make_unique<CreateNodeOp>(values[0], rootNodeId));
        operations.push_back(std::make_unique<SetRootOp>(rootNodeId));  // Set as root immediately
        nodeIds.push_back(rootNodeId);

        // Create remaining nodes and connect them immediately
        for (size_t i = 1; i < values.size(); ++i) {
            int nodeId = tree.allocateNodeId();

            // Create the node (goes into temp slot)
            operations.push_back(std::make_unique<CreateNodeOp>(values[i], nodeId));

            // Determine parent and which child (left or right)
            size_t parentIdx = (i - 1) / 2;
            bool isLeftChild = (i % 2 == 1);

            // Connect immediately (removes from temp slot)
            operations.push_back(std::make_unique<ConnectOp>(nodeIds[parentIdx], nodeId, isLeftChild));

            nodeIds.push_back(nodeId);
        }
    }
};
