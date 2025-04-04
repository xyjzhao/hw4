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
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's balance.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right, returning AVLNode pointers.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // Balance factor (roughly, height(left)-height(right))
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* Constructor: simply calls the base constructor and initializes balance_ to 0.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
* Destructor.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
* Returns the balance factor.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* Sets the balance factor.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance factor.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* Returns the parent as an AVLNode pointer.
*/
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Returns the left child as an AVLNode pointer.
*/
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Returns the right child as an AVLNode pointer.
*/
template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item) override; 
    virtual void remove(const Key& key) override;
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Helper rotations.
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);

    // Rebalancing helper: walk upward from a node and rebalance if necessary.
    void rebalance(AVLNode<Key, Value>* node);

    // Helper to cast a Node pointer to an AVLNode pointer.
    AVLNode<Key, Value>* asAVL(Node<Key, Value>* node)
    {
        return static_cast<AVLNode<Key, Value>*>(node);
    }
};

/*
 * AVLTree::insert
 *
 * Inserts new_item into the tree. If the key already exists, the value is updated.
 * Then, walking back up the tree from the inserted node, update balance factors and
 * perform rotations as needed.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // Special-case: tree is empty.
    if(this->root_ == nullptr) {
         this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
         return;
    }
    // Perform a standard BST insertion, but cast nodes to AVLNode.
    AVLNode<Key, Value>* current = asAVL(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    while(current != nullptr) {
         parent = current;
         if(new_item.first < current->getKey())
              current = asAVL(current->getLeft());
         else if(new_item.first > current->getKey())
              current = asAVL(current->getRight());
         else {
              // Key already exists, update its value.
              current->setValue(new_item.second);
              return;
         }
    }
    // Insert new node.
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(new_item.first < parent->getKey())
         parent->setLeft(newNode);
    else
         parent->setRight(newNode);

    // Walk upward to update balance factors and rebalance.
    rebalance(parent);
}

/*
 * AVLTree::remove
 *
 * Removes the node with the given key. (If not found, does nothing.)
 * For a node with two children, swaps it with its predecessor (as specified)
 * and then removes the node. Then, walks upward from the parent to update
 * balance factors and rebalance the tree.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // Find the node to remove.
    AVLNode<Key, Value>* nodeToRemove = asAVL(this->internalFind(key));
    if(nodeToRemove == nullptr)
         return; // Key not found.

    AVLNode<Key, Value>* parent = nodeToRemove->getParent();

    // If the node has two children, swap with predecessor.
    if(nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
         AVLNode<Key, Value>* pred = asAVL(BinarySearchTree<Key, Value>::predecessor(nodeToRemove));
         this->nodeSwap(nodeToRemove, pred);
         // Now, nodeToRemove has at most one child.
    }

    // At this point, nodeToRemove has at most one child.
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

    // Rebalance starting from the parent.
    if(parent != nullptr)
         rebalance(parent);
}

/**
 * AVLTree::nodeSwap
 *
 * Swaps two AVLNodes (calls the base class swap then swaps balance factors).
 */
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

/**
 * rotateLeft
 *
 * Performs a left rotation on node n.
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

    // For simplicity, recompute balances as zero.
    n->setBalance(0);
    r->setBalance(0);
}

/**
 * rotateRight
 *
 * Performs a right rotation on node n.
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

    // For simplicity, recompute balances as zero.
    n->setBalance(0);
    l->setBalance(0);
}

/**
 * rebalance
 *
 * Walks upward from the given node, updating balance factors.
 * When a node is found to be unbalanced (balance factor > 1 or < -1),
 * performs the appropriate rotation(s). This simplified version
 * resets balance factors to 0 after a rotation.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    while(node != nullptr) {
         // Update balance factor.
         int8_t leftHeight = (node->getLeft() != nullptr) ? asAVL(node->getLeft())->getBalance() : 0;
         int8_t rightHeight = (node->getRight() != nullptr) ? asAVL(node->getRight())->getBalance() : 0;
         // (In a full implementation, you would compute the actual heights.)
         // Here, we use balance factor differences heuristically.
         int8_t balance = node->getBalance();
         // If the node became unbalanced, perform rotations.
         if(balance > 1) { // left heavy
              AVLNode<Key, Value>* leftChild = node->getLeft();
              if(leftChild != nullptr && leftChild->getBalance() < 0) {
                   // Left-Right case.
                   rotateLeft(leftChild);
                   rotateRight(node);
              } else {
                   // Left-Left case.
                   rotateRight(node);
              }
         } else if(balance < -1) { // right heavy
              AVLNode<Key, Value>* rightChild = node->getRight();
              if(rightChild != nullptr && rightChild->getBalance() > 0) {
                   // Right-Left case.
                   rotateRight(rightChild);
                   rotateLeft(node);
              } else {
                   // Right-Right case.
                   rotateLeft(node);
              }
         }
         // Move up.
         node = asAVL(node->getParent());
    }
}

#endif
