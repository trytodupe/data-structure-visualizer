#pragma once
#include "../core/DataStructure.h"
#include <memory>
#include <string>
#include <sstream>

/**
 * Tree node structure for binary tree implementation.
 */
struct TreeNode {
    int value;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;
    TreeNode* parent;

    TreeNode(int val) : value(val), parent(nullptr) {}
};

/**
 * Binary tree data structure implementation.
 * Provides basic tree operations and traversal.
 */
class TreeStructure : public DataStructure {
public:
    std::unique_ptr<TreeNode> root;
    size_t nodeCount = 0;

    TreeStructure() = default;

    void initialize() override {
        root.reset();
        nodeCount = 0;
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
};
