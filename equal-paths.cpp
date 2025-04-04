#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// Helper function to get heights of all paths
bool checkEqualPaths(Node* root, int& height, int currentHeight) {
    // Base case: empty tree
    if (root == nullptr) {
        return true;
    }
    
    // If this is a leaf node
    if (root->left == nullptr && root->right == nullptr) {
        // If this is the first leaf we've found, set the height
        if (height == -1) {
            height = currentHeight;
            return true;
        }
        // Otherwise, check if this leaf's height matches the stored height
        return height == currentHeight;
    }
    
    // Recursively check left and right subtrees
    bool leftEqual = true;
    bool rightEqual = true;
    
    if (root->left != nullptr) {
        leftEqual = checkEqualPaths(root->left, height, currentHeight + 1);
    }
    
    if (root->right != nullptr) {
        rightEqual = checkEqualPaths(root->right, height, currentHeight + 1);
    }
    
    // Both subtrees must have equal paths
    return leftEqual && rightEqual;
}

bool equalPaths(Node* root) {
    // Empty tree has equal paths
    if (root == nullptr) {
        return true;
    }
    
    // Use -1 as a sentinel value for "height not yet determined"
    int height = -1;
    return checkEqualPaths(root, height, 1);
}