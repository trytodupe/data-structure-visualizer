#pragma once
#include "../core/DataStructure.h"
#include "imgui.h"
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <map>

/**
 * Tree node structure for binary tree implementation.
 */
struct TreeNode {
    int value;
    TreeNode* left;   // Changed to raw pointers for more flexible memory management
    TreeNode* right;
    TreeNode* parent;
    int id;  // Unique identifier for each node

    TreeNode(int val, int nodeId)
        : value(val), left(nullptr), right(nullptr), parent(nullptr), id(nodeId) {}
};

/**
 * Binary tree data structure implementation.
 * Provides basic tree operations and traversal.
 */
class BinaryTreeStructure : public DataStructure {
private:
    int nextNodeId;  // Counter for assigning unique IDs to nodes
    std::map<int, TreeNode*> nodeRegistry;  // Map of node ID to node pointer

public:
    TreeNode* root;
    TreeNode* tempSlot;  // Single temporary detached node for visualization
    size_t nodeCount;

    BinaryTreeStructure() : nextNodeId(0), root(nullptr), tempSlot(nullptr), nodeCount(0) {}

    ~BinaryTreeStructure() {
        // Clean up all allocated nodes
        clear();
    }

    void initialize() override {
        clear();
    }

    void clear() {
        // Delete all nodes in registry
        for (auto& pair : nodeRegistry) {
            delete pair.second;
        }
        nodeRegistry.clear();
        tempSlot = nullptr;
        root = nullptr;
        nodeCount = 0;
        nextNodeId = 0;
    }

    std::string getType() const override {
        return "BinaryTree";
    }

    std::string getStateDescription() const override {
        std::ostringstream oss;
        oss << "Tree[" << nodeCount << " nodes]";
        if (root) {
            oss << " root=" << root->value;
        } else {
            oss << " (empty)";
        }
        return oss.str();
    }

    /**
     * Draw the binary tree visualization
     * @param startPos Starting position for drawing
     * @param boxSize Size of each node box
     * @param spacing Spacing between nodes
     * @return Size of the drawn area
     */
    ImVec2 draw(ImVec2 startPos, float boxSize, float spacing) const override {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Get available space for dynamic centering
        ImVec2 availableSpace = ImGui::GetContentRegionAvail();
        float centerX = startPos.x + availableSpace.x * 0.5f;

        float maxWidth = 0.0f;
        float maxHeight = 0.0f;

        if (root) {
            // Draw main tree - start with root at center, initial horizontal spacing
            float initialHSpacing = 150.0f;
            drawNode(drawList, root, centerX, startPos.y + 20.0f, initialHSpacing, boxSize, spacing, maxWidth, maxHeight);
        } else {
            // Draw empty tree indicator
            const char* emptyText = "(empty tree)";
            ImVec2 textSize = ImGui::CalcTextSize(emptyText);
            ImVec2 textPos(centerX - textSize.x * 0.5f, startPos.y + 20.0f);
            drawList->AddText(textPos, IM_COL32(150, 150, 150, 255), emptyText);
            maxWidth = textSize.x;
            maxHeight = textSize.y;
        }

        // Draw temp slot below the main tree with minimal spacing
        float tempSlotY = startPos.y + maxHeight + 40.0f;
        drawList->AddText(ImVec2(startPos.x, tempSlotY), IM_COL32(200, 200, 200, 255), "Temp Slot:");
        tempSlotY += 25.0f;

        float x = startPos.x;
        // Draw temp slot box (empty or with node)
        ImVec2 boxMin(x, tempSlotY);
        ImVec2 boxMax(x + boxSize, tempSlotY + boxSize);

        if (tempSlot) {
            // Draw node in temp slot
            drawList->AddRectFilled(boxMin, boxMax, IM_COL32(80, 120, 180, 255));
            drawList->AddRect(boxMin, boxMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

            // Draw value
            char valueText[16];
            snprintf(valueText, sizeof(valueText), "%d", tempSlot->value);
            ImVec2 textSize = ImGui::CalcTextSize(valueText);
            ImVec2 textPos(x + (boxSize - textSize.x) * 0.5f, tempSlotY + (boxSize - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), valueText);

            // Draw ID below
            char idText[16];
            snprintf(idText, sizeof(idText), "ID:%d", tempSlot->id);
            ImVec2 idSize = ImGui::CalcTextSize(idText);
            ImVec2 idPos(x + (boxSize - idSize.x) * 0.5f, tempSlotY + boxSize + 5.0f);
            drawList->AddText(idPos, IM_COL32(150, 150, 150, 255), idText);
        } else {
            // Draw empty slot
            drawList->AddRect(boxMin, boxMax, IM_COL32(100, 100, 100, 255), 0.0f, 0, 2.0f);
            const char* emptyText = "(empty)";
            ImVec2 textSize = ImGui::CalcTextSize(emptyText);
            ImVec2 textPos(x + (boxSize - textSize.x) * 0.5f, tempSlotY + (boxSize - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(100, 100, 100, 255), emptyText);
        }

        maxHeight = tempSlotY + boxSize + 5.0f - startPos.y;

        return ImVec2(availableSpace.x, maxHeight);
    }

    /**
     * Register a node in the registry
     */
    void registerNode(TreeNode* node) {
        nodeRegistry[node->id] = node;
        nodeCount++;
    }

    /**
     * Unregister a node from the registry (does not delete)
     */
    void unregisterNode(TreeNode* node) {
        nodeRegistry.erase(node->id);
        nodeCount--;
    }

    /**
     * Get a node by its ID
     */
    TreeNode* getNodeById(int id) const {
        auto it = nodeRegistry.find(id);
        return (it != nodeRegistry.end()) ? it->second : nullptr;
    }

    /**
     * Allocate a new node ID
     */
    int allocateNodeId() {
        return nextNodeId++;
    }

    /**
     * Add a node to temp slot
     */
    void addToTempSlot(TreeNode* node) {
        tempSlot = node;
    }

    /**
     * Remove a node from temp slot
     */
    void removeFromTempSlot(TreeNode* node) {
        if (tempSlot == node) {
            tempSlot = nullptr;
        }
    }

private:
    /**
     * Recursively draw a tree node and its children
     */
    void drawNode(ImDrawList* drawList, TreeNode* node, float x, float y, float hSpacing,
                   float boxSize, float vSpacing, float& maxWidth, float& maxHeight) const {
        if (!node) return;

        // Update max dimensions
        maxWidth = std::max(maxWidth, x + boxSize);
        maxHeight = std::max(maxHeight, y + boxSize);

        // Draw node box
        ImVec2 boxMin(x - boxSize * 0.5f, y);
        ImVec2 boxMax(x + boxSize * 0.5f, y + boxSize);
        drawList->AddRectFilled(boxMin, boxMax, IM_COL32(100, 150, 200, 255));
        drawList->AddRect(boxMin, boxMax, IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);

        // Draw value
        char valueText[16];
        snprintf(valueText, sizeof(valueText), "%d", node->value);
        ImVec2 textSize = ImGui::CalcTextSize(valueText);
        ImVec2 textPos(x - textSize.x * 0.5f, y + (boxSize - textSize.y) * 0.5f);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), valueText);

        // Draw children connections and nodes with shorter vertical spacing
        float childY = y + boxSize + 20.0f;  // Reduced from vSpacing + 50.0f

        if (node->left) {
            float leftX = x - hSpacing;
            // Draw line to left child
            ImVec2 parentBottom(x, y + boxSize);
            ImVec2 childTop(leftX, childY);
            drawList->AddLine(parentBottom, childTop, IM_COL32(200, 200, 200, 255), 2.0f);
            // Recursively draw left subtree
            drawNode(drawList, node->left, leftX, childY, hSpacing * 0.5f, boxSize, vSpacing, maxWidth, maxHeight);
        }

        if (node->right) {
            float rightX = x + hSpacing;
            // Draw line to right child
            ImVec2 parentBottom(x, y + boxSize);
            ImVec2 childTop(rightX, childY);
            drawList->AddLine(parentBottom, childTop, IM_COL32(200, 200, 200, 255), 2.0f);
            // Recursively draw right subtree
            drawNode(drawList, node->right, rightX, childY, hSpacing * 0.5f, boxSize, vSpacing, maxWidth, maxHeight);
        }
    }
};
