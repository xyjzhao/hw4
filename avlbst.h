#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
 * A special kind of node for an AVL tree. It extends the BST Node by adding
 * a balance factor (balance_ = height(left subtree) – height(right subtree)).
 */
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    int8_t getBalance() const;
    void setBalance(int8_t balance);
    void updateBalance(int8_t diff);

    // Overridden getters that return AVLNode pointers.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_; // balance factor = height(left) - height(right)
};

/* --- AVLNode implementations --- */

template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent)
    : Node<Key, Value>(key, value, parent), balance_(0)
{ }

template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode() { }

template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/**
 * computeHeight: A free helper that returns the height of an AVLNode subtree.
 * The height of a nullptr is 0.
 */
template <class Key, class Value>
int computeHeight(AVLNode<Key, Value>* node) {
    if(node == nullptr)
        return 0;
    return std::max(computeHeight(node->getLeft()), computeHeight(node->getRight())) + 1;
}

/**
 * AVLTree extends BinarySearchTree with AVL rebalancing.
 */
template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item) override;
    virtual void remove(const Key& key) override;
protected:
    // Our custom nodeSwap override.
    virtual void nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Rotation helpers.
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);

    // Recalculates and sets the balance factor for a node.
    void updateBalance(AVLNode<Key, Value>* node);

    // Rebalances upward from the given node.
    void rebalance(AVLNode<Key, Value>* node);

    // Helper to cast a Node pointer to an AVLNode pointer.
    AVLNode<Key, Value>* asAVL(Node<Key, Value>* node) {
        return static_cast<AVLNode<Key, Value>*>(node);
    }
};

/* --- AVLTree implementations --- */

/**
 * AVLTree::insert
 *
 * Performs a standard BST insertion using AVLNode objects.
 * After insertion, walks upward from the parent and rebalances.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if(this->root_ == nullptr) {
         this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
         return;
    }
    AVLNode<Key, Value>* current = asAVL(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    while(current != nullptr) {
         parent = current;
         if(new_item.first < current->getKey())
              current = asAVL(current->getLeft());
         else if(new_item.first > current->getKey())
              current = asAVL(current->getRight());
         else {
              // Key exists; update its value.
              current->setValue(new_item.second);
              return;
         }
    }
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(new_item.first < parent->getKey())
         parent->setLeft(newNode);
    else
         parent->setRight(newNode);

    // Rebalance upward from the parent.
    rebalance(parent);
}

/**
 * AVLTree::remove
 *
 * Removes the node with the given key.
 * If the node has two children, finds its predecessor and swaps it with the node.
 * Then removes the original node (which now occupies the predecessor’s position).
 * Finally, rebalances upward from the (possibly new) parent.
 *
 * (Key change: We do not update the pointer to remove after swapping.)
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* nodeToRemove = asAVL(this->internalFind(key));
    if(nodeToRemove == nullptr)
         return; // key not found

    AVLNode<Key, Value>* parent = nodeToRemove->getParent();

    if(nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
         // Find predecessor.
         AVLNode<Key, Value>* pred = asAVL(BinarySearchTree<Key, Value>::predecessor(nodeToRemove));
         // Swap nodeToRemove and its predecessor.
         this->nodeSwap(nodeToRemove, pred);
         // Do not update nodeToRemove—remove the same node (which now holds the predecessor's key).
         // Parent pointer might change.
         parent = nodeToRemove->getParent();
    }

    AVLNode<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ?
                                   asAVL(nodeToRemove->getLeft()) : asAVL(nodeToRemove->getRight());
    if(child != nullptr)
         child->setParent(parent);
    if(parent == nullptr)
         this->root_ = child;
    else {
         if(parent->getLeft() == nodeToRemove)
              parent->setLeft(child);
         else
              parent->setRight(child);
    }
    delete nodeToRemove;

    if(parent != nullptr)
         rebalance(parent);
}

/**
 * AVLTree::nodeSwap
 *
 * Swaps two AVLNodes. If one is the parent of the other (adjacent swap),
 * handles that case by rearranging pointers so that the node with the predecessor's
 * key moves into the position of the node to remove.
 * Otherwise, delegates to the base class nodeSwap.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    // Check if n1 is parent of n2.
    if(n1->getLeft() == n2) {
         AVLNode<Key, Value>* P = n1->getParent();
         AVLNode<Key, Value>* B = n2->getRight();  // n2's right subtree
         n2->setParent(P);
         if(P != nullptr) {
             if(P->getLeft() == n1)
                  P->setLeft(n2);
             else
                  P->setRight(n2);
         } else {
             this->root_ = n2;
         }
         n2->setRight(n1);
         n1->setParent(n2);
         n1->setLeft(B);
         if(B != nullptr)
              B->setParent(n1);
         int8_t tempB = n1->getBalance();
         n1->setBalance(n2->getBalance());
         n2->setBalance(tempB);
         return;
    }
    else if(n1->getRight() == n2) {
         AVLNode<Key, Value>* P = n1->getParent();
         AVLNode<Key, Value>* B = n2->getLeft();  // n2's left subtree
         n2->setParent(P);
         if(P != nullptr) {
             if(P->getLeft() == n1)
                  P->setLeft(n2);
             else
                  P->setRight(n2);
         } else {
             this->root_ = n2;
         }
         n2->setLeft(n1);
         n1->setParent(n2);
         n1->setRight(B);
         if(B != nullptr)
              B->setParent(n1);
         int8_t tempB = n1->getBalance();
         n1->setBalance(n2->getBalance());
         n2->setBalance(tempB);
         return;
    }
    else {
         // Non-adjacent: delegate to base class swap.
         BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
         int8_t tempB = n1->getBalance();
         n1->setBalance(n2->getBalance());
         n2->setBalance(tempB);
         return;
    }
}

/**
 * rotateLeft
 *
 * Performs a left rotation on node n.
 * After rotation, updates balance factors.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* r = n->getRight();
    n->setRight(r->getLeft());
    if(r->getLeft() != nullptr)
         r->getLeft()->setParent(n);
    r->setParent(n->getParent());
    if(n->getParent() == nullptr)
         this->root_ = r;
    else if(n == n->getParent()->getLeft())
         n->getParent()->setLeft(r);
    else
         n->getParent()->setRight(r);
    r->setLeft(n);
    n->setParent(r);

    updateBalance(n);
    updateBalance(r);
}

/**
 * rotateRight
 *
 * Performs a right rotation on node n.
 * After rotation, updates balance factors.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* n)
{
    AVLNode<Key, Value>* l = n->getLeft();
    n->setLeft(l->getRight());
    if(l->getRight() != nullptr)
         l->getRight()->setParent(n);
    l->setParent(n->getParent());
    if(n->getParent() == nullptr)
         this->root_ = l;
    else if(n == n->getParent()->getRight())
         n->getParent()->setRight(l);
    else
         n->getParent()->setLeft(l);
    l->setRight(n);
    n->setParent(l);

    updateBalance(n);
    updateBalance(l);
}

/**
 * updateBalance
 *
 * Recalculates the balance factor for node as:
 *   height(left subtree) - height(right subtree)
 */
template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node)
{
    if(node == nullptr) return;
    int leftHeight = (node->getLeft() == nullptr) ? 0 : computeHeight(asAVL(node->getLeft()));
    int rightHeight = (node->getRight() == nullptr) ? 0 : computeHeight(asAVL(node->getRight()));
    node->setBalance(leftHeight - rightHeight);
}

/**
 * rebalance
 *
 * Walks upward from node, updating balance factors and performing rotations as needed.
 * Uses computeHeight to determine subtree heights.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    while(node != nullptr) {
         updateBalance(node);
         int balance = node->getBalance();
         if(balance > 1) { // left heavy
              AVLNode<Key, Value>* leftChild = node->getLeft();
              if(leftChild != nullptr && computeHeight(asAVL(leftChild->getLeft())) < computeHeight(asAVL(leftChild->getRight()))) {
                   // Left-Right case.
                   rotateLeft(leftChild);
                   rotateRight(node);
              } else {
                   // Left-Left case.
                   rotateRight(node);
              }
         } else if(balance < -1) { // right heavy
              AVLNode<Key, Value>* rightChild = node->getRight();
              if(rightChild != nullptr && computeHeight(asAVL(rightChild->getRight())) < computeHeight(asAVL(rightChild->getLeft()))) {
                   // Right-Left case.
                   rotateRight(rightChild);
                   rotateLeft(node);
              } else {
                   // Right-Right case.
                   rotateLeft(node);
              }
         }
         node = asAVL(node->getParent());
    }
}

#endif
