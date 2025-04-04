#ifndef BST_H
#define BST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <utility>
#include <algorithm>  // for std::max
#include <cmath>      // for std::abs

/**
 * A templated class for a Node in a search tree.
 * The getters for parent/left/right are virtual so
 * that they can be overridden for future kinds of
 * search trees, such as Red Black trees, Splay trees,
 * and AVL trees.
 */
template <typename Key, typename Value>
class Node
{
public:
    Node(const Key& key, const Value& value, Node<Key, Value>* parent);
    virtual ~Node();

    const std::pair<const Key, Value>& getItem() const;
    std::pair<const Key, Value>& getItem();
    const Key& getKey() const;
    const Value& getValue() const;
    Value& getValue();

    virtual Node<Key, Value>* getParent() const;
    virtual Node<Key, Value>* getLeft() const;
    virtual Node<Key, Value>* getRight() const;

    void setParent(Node<Key, Value>* parent);
    void setLeft(Node<Key, Value>* left);
    void setRight(Node<Key, Value>* right);
    void setValue(const Value &value);

protected:
    std::pair<const Key, Value> item_;
    Node<Key, Value>* parent_;
    Node<Key, Value>* left_;
    Node<Key, Value>* right_;
};

/*
  -----------------------------------------
  Begin implementations for the Node class.
  -----------------------------------------
*/

/**
* Explicit constructor for a node.
*/
template<typename Key, typename Value>
Node<Key, Value>::Node(const Key& key, const Value& value, Node<Key, Value>* parent) :
    item_(key, value),
    parent_(parent),
    left_(nullptr),
    right_(nullptr)
{
}

/**
* Destructor.
*/
template<typename Key, typename Value>
Node<Key, Value>::~Node()
{
}

/**
* A const getter for the item.
*/
template<typename Key, typename Value>
const std::pair<const Key, Value>& Node<Key, Value>::getItem() const
{
    return item_;
}

/**
* A non-const getter for the item.
*/
template<typename Key, typename Value>
std::pair<const Key, Value>& Node<Key, Value>::getItem()
{
    return item_;
}

/**
* A const getter for the key.
*/
template<typename Key, typename Value>
const Key& Node<Key, Value>::getKey() const
{
    return item_.first;
}

/**
* A const getter for the value.
*/
template<typename Key, typename Value>
const Value& Node<Key, Value>::getValue() const
{
    return item_.second;
}

/**
* A non-const getter for the value.
*/
template<typename Key, typename Value>
Value& Node<Key, Value>::getValue()
{
    return item_.second;
}

/**
* Returns the parent of the node.
*/
template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getParent() const
{
    return parent_;
}

/**
* Returns the left child.
*/
template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getLeft() const
{
    return left_;
}

/**
* Returns the right child.
*/
template<typename Key, typename Value>
Node<Key, Value>* Node<Key, Value>::getRight() const
{
    return right_;
}

/**
* Sets the parent pointer.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setParent(Node<Key, Value>* parent)
{
    parent_ = parent;
}

/**
* Sets the left child pointer.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setLeft(Node<Key, Value>* left)
{
    left_ = left;
}

/**
* Sets the right child pointer.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setRight(Node<Key, Value>* right)
{
    right_ = right;
}

/**
* Sets the node's value.
*/
template<typename Key, typename Value>
void Node<Key, Value>::setValue(const Value& value)
{
    item_.second = value;
}

/*
  ---------------------------------------
  End implementations for the Node class.
  ---------------------------------------
*/

/**
* A templated unbalanced binary search tree.
*/
template <typename Key, typename Value>
class BinarySearchTree
{
public:
    BinarySearchTree(); // Constructor
    virtual ~BinarySearchTree(); // Destructor
    virtual void insert(const std::pair<const Key, Value>& keyValuePair);
    virtual void remove(const Key& key);
    void clear();
    bool isBalanced() const;
    void print() const;
    bool empty() const;

    template<typename PPKey, typename PPValue>
    friend void prettyPrintBST(BinarySearchTree<PPKey, PPValue> & tree);
public:
    /**
    * An internal iterator class for traversing the BST.
    */
    class iterator
    {
    public:
        iterator();

        std::pair<const Key,Value>& operator*() const;
        std::pair<const Key,Value>* operator->() const;

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

        iterator& operator++();

    protected:
        friend class BinarySearchTree<Key, Value>;
        iterator(Node<Key,Value>* ptr);
        Node<Key, Value>* current_;
    };

public:
    iterator begin() const;
    iterator end() const;
    iterator find(const Key& key) const;
    Value& operator[](const Key& key);
    Value const & operator[](const Key& key) const;

protected:
    // Mandatory helper functions
    Node<Key, Value>* internalFind(const Key& k) const;
    Node<Key, Value>* getSmallestNode() const;
    static Node<Key, Value>* predecessor(Node<Key, Value>* current);
    static Node<Key, Value>* successor(Node<Key, Value>* current);

    // Helper function to check subtree height (returns -1 if unbalanced)
    int checkHeight(Node<Key, Value>* node) const;

    // Provided helper functions
    virtual void printRoot(Node<Key, Value>* r) const;
    virtual void nodeSwap(Node<Key, Value>* n1, Node<Key, Value>* n2);

protected:
    Node<Key, Value>* root_;
};

/*
--------------------------------------------------------------
Begin implementations for the BinarySearchTree::iterator class.
--------------------------------------------------------------
*/

/**
* Constructs an iterator from a given node pointer.
*/
template<class Key, class Value>
BinarySearchTree<Key, Value>::iterator::iterator(Node<Key,Value>* ptr)
{
    current_ = ptr;
}

/**
* Default constructor for an iterator.
*/
template<class Key, class Value>
BinarySearchTree<Key, Value>::iterator::iterator() 
{
    current_ = nullptr;
}

/**
* Dereference operator.
*/
template<class Key, class Value>
std::pair<const Key,Value>& BinarySearchTree<Key, Value>::iterator::operator*() const
{
    return current_->getItem();
}

/**
* Arrow operator.
*/
template<class Key, class Value>
std::pair<const Key,Value>* BinarySearchTree<Key, Value>::iterator::operator->() const
{
    return &(current_->getItem());
}

/**
* Equality operator.
*/
template<class Key, class Value>
bool BinarySearchTree<Key, Value>::iterator::operator==(const BinarySearchTree<Key, Value>::iterator& rhs) const
{
    return current_ == rhs.current_;
}

/**
* Inequality operator.
*/
template<class Key, class Value>
bool BinarySearchTree<Key, Value>::iterator::operator!=(const BinarySearchTree<Key, Value>::iterator& rhs) const
{
    return current_ != rhs.current_;
}

/**
* Pre-increment operator (in-order traversal).
*/
template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator& BinarySearchTree<Key, Value>::iterator::operator++()
{
    current_ = BinarySearchTree<Key, Value>::successor(current_);
    return *this;
}

/*
-------------------------------------------------------------
End implementations for the BinarySearchTree::iterator class.
-------------------------------------------------------------
*/

/*
-----------------------------------------------------
Begin implementations for the BinarySearchTree class.
-----------------------------------------------------
*/

/**
* Constructor initializes the tree as empty.
*/
template<class Key, class Value>
BinarySearchTree<Key, Value>::BinarySearchTree() 
{
    root_ = nullptr;
}

/**
* Destructor clears the tree.
*/
template<typename Key, typename Value>
BinarySearchTree<Key, Value>::~BinarySearchTree()
{
    clear();
}

/**
 * Returns true if the tree is empty.
*/
template<class Key, class Value>
bool BinarySearchTree<Key, Value>::empty() const
{
    return root_ == nullptr;
}

template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::print() const
{
    printRoot(root_);
    std::cout << "\n";
}

/**
* Returns an iterator to the smallest item in the tree.
*/
template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator BinarySearchTree<Key, Value>::begin() const
{
    BinarySearchTree<Key, Value>::iterator begin(getSmallestNode());
    return begin;
}

/**
* Returns an iterator representing the end.
*/
template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator BinarySearchTree<Key, Value>::end() const
{
    BinarySearchTree<Key, Value>::iterator end(nullptr);
    return end;
}

/**
* Finds the node with the given key and returns an iterator to it.
*/
template<class Key, class Value>
typename BinarySearchTree<Key, Value>::iterator BinarySearchTree<Key, Value>::find(const Key & k) const
{
    Node<Key, Value>* curr = internalFind(k);
    BinarySearchTree<Key, Value>::iterator it(curr);
    return it;
}

/**
 * Returns the value associated with the given key.
 * Throws std::out_of_range if the key is not found.
 */
template<class Key, class Value>
Value& BinarySearchTree<Key, Value>::operator[](const Key& key)
{
    Node<Key, Value>* curr = internalFind(key);
    if(curr == nullptr) throw std::out_of_range("Invalid key");
    return curr->getValue();
}

template<class Key, class Value>
Value const & BinarySearchTree<Key, Value>::operator[](const Key& key) const
{
    Node<Key, Value>* curr = internalFind(key);
    if(curr == nullptr) throw std::out_of_range("Invalid key");
    return curr->getValue();
}

/**
* Inserts a key/value pair into the BST.
* If the key already exists, updates its value.
*/
template<class Key, class Value>
void BinarySearchTree<Key, Value>::insert(const std::pair<const Key, Value>& keyValuePair)
{
    if(root_ == nullptr) {
         root_ = new Node<Key, Value>(keyValuePair.first, keyValuePair.second, nullptr);
         return;
    }
    Node<Key, Value>* current = root_;
    Node<Key, Value>* parent = nullptr;
    while(current != nullptr) {
         parent = current;
         if(keyValuePair.first < current->getKey())
              current = current->getLeft();
         else if(keyValuePair.first > current->getKey())
              current = current->getRight();
         else {
              // Key exists; update the value.
              current->setValue(keyValuePair.second);
              return;
         }
    }
    Node<Key, Value>* newNode = new Node<Key, Value>(keyValuePair.first, keyValuePair.second, parent);
    if(keyValuePair.first < parent->getKey())
         parent->setLeft(newNode);
    else
         parent->setRight(newNode);
}

/**
* Removes the node with the given key from the BST.
* If the node has two children, swaps it with its predecessor before removal.
*/
template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::remove(const Key& key)
{
    Node<Key, Value>* nodeToRemove = internalFind(key);
    if(nodeToRemove == nullptr)
         return;  // Key not found

    // If node has two children, swap with its predecessor.
    if(nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
         Node<Key, Value>* pred = predecessor(nodeToRemove);
         nodeSwap(nodeToRemove, pred);
    }

    // Now nodeToRemove has at most one child.
    Node<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ? nodeToRemove->getLeft() : nodeToRemove->getRight();
    Node<Key, Value>* parent = nodeToRemove->getParent();

    if(child != nullptr)
         child->setParent(parent);
    if(parent == nullptr)
         root_ = child;
    else {
         if(parent->getLeft() == nodeToRemove)
              parent->setLeft(child);
         else
              parent->setRight(child);
    }
    delete nodeToRemove;
}

/**
* Removes all nodes from the BST.
*/
template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::clear()
{
    // clearHelper is defined below.
    clearHelper(root_);
    root_ = nullptr;
}

/**
* Returns true if the BST is balanced.
* (A balanced tree has the height difference between left and right subtrees ≤ 1 at every node.)
*/
template<typename Key, typename Value>
bool BinarySearchTree<Key, Value>::isBalanced() const
{
    return (checkHeight(root_) != -1);
}

/**
* Returns the smallest node in the BST.
*/
template<typename Key, typename Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::getSmallestNode() const
{
    Node<Key, Value>* current = root_;
    if(current == nullptr)
         return nullptr;
    while(current->getLeft() != nullptr)
         current = current->getLeft();
    return current;
}

/**
* Finds and returns the node with the given key.
* Returns nullptr if not found.
*/
template<typename Key, typename Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::internalFind(const Key& key) const
{
    Node<Key, Value>* current = root_;
    while(current != nullptr) {
         if(key < current->getKey())
              current = current->getLeft();
         else if(key > current->getKey())
              current = current->getRight();
         else
              return current;
    }
    return nullptr;
}

/**
* Returns the predecessor of the given node (in-order).
*/
template<class Key, class Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::predecessor(Node<Key, Value>* current)
{
    if(current == nullptr)
         return nullptr;
    if(current->getLeft() != nullptr) {
         Node<Key, Value>* temp = current->getLeft();
         while(temp->getRight() != nullptr)
              temp = temp->getRight();
         return temp;
    } else {
         Node<Key, Value>* parent = current->getParent();
         while(parent != nullptr && current == parent->getLeft()) {
              current = parent;
              parent = parent->getParent();
         }
         return parent;
    }
}

/**
* Returns the successor of the given node (in-order).
*/
template<class Key, class Value>
Node<Key, Value>* BinarySearchTree<Key, Value>::successor(Node<Key, Value>* current)
{
    if(current == nullptr)
         return nullptr;
    if(current->getRight() != nullptr) {
         Node<Key, Value>* temp = current->getRight();
         while(temp->getLeft() != nullptr)
              temp = temp->getLeft();
         return temp;
    } else {
         Node<Key, Value>* parent = current->getParent();
         while(parent != nullptr && current == parent->getRight()) {
              current = parent;
              parent = parent->getParent();
         }
         return parent;
    }
}

/**
 * Recursively computes the height of the subtree.
 * Returns -1 if the subtree is unbalanced.
 */
template<typename Key, typename Value>
int BinarySearchTree<Key, Value>::checkHeight(Node<Key, Value>* node) const {
    if(node == nullptr)
        return 0;
    int leftHeight = checkHeight(node->getLeft());
    if(leftHeight == -1)
        return -1;
    int rightHeight = checkHeight(node->getRight());
    if(rightHeight == -1)
        return -1;
    if(std::abs(leftHeight - rightHeight) > 1)
        return -1;
    return std::max(leftHeight, rightHeight) + 1;
}

/**
 * Swaps two nodes in the BST.
 */
template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::nodeSwap(Node<Key, Value>* n1, Node<Key, Value>* n2)
{
    if((n1 == n2) || (n1 == nullptr) || (n2 == nullptr))
        return;
    Node<Key, Value>* n1p = n1->getParent();
    Node<Key, Value>* n1r = n1->getRight();
    Node<Key, Value>* n1lt = n1->getLeft();
    bool n1isLeft = (n1p != nullptr && n1 == n1p->getLeft());
    Node<Key, Value>* n2p = n2->getParent();
    Node<Key, Value>* n2r = n2->getRight();
    Node<Key, Value>* n2lt = n2->getLeft();
    bool n2isLeft = (n2p != nullptr && n2 == n2p->getLeft());

    Node<Key, Value>* temp;
    temp = n1->getParent();
    n1->setParent(n2->getParent());
    n2->setParent(temp);

    temp = n1->getLeft();
    n1->setLeft(n2->getLeft());
    n2->setLeft(temp);

    temp = n1->getRight();
    n1->setRight(n2->getRight());
    n2->setRight(temp);

    if(n1r != nullptr && n1r == n2) {
        n2->setRight(n1);
        n1->setParent(n2);
    }
    else if(n2r != nullptr && n2r == n1) {
        n1->setRight(n2);
        n2->setParent(n1);
    }
    else if(n1lt != nullptr && n1lt == n2) {
        n2->setLeft(n1);
        n1->setParent(n2);
    }
    else if(n2lt != nullptr && n2lt == n1) {
        n1->setLeft(n2);
        n2->setParent(n1);
    }

    if(n1p != nullptr && n1p != n2) {
        if(n1isLeft)
            n1p->setLeft(n2);
        else
            n1p->setRight(n2);
    }
    if(n1r != nullptr && n1r != n2)
         n1r->setParent(n2);
    if(n1lt != nullptr && n1lt != n2)
         n1lt->setParent(n2);

    if(n2p != nullptr && n2p != n1) {
        if(n2isLeft)
            n2p->setLeft(n1);
        else
            n2p->setRight(n1);
    }
    if(n2r != nullptr && n2r != n1)
         n2r->setParent(n1);
    if(n2lt != nullptr && n2lt != n1)
         n2lt->setParent(n1);

    if(this->root_ == n1)
         this->root_ = n2;
    else if(this->root_ == n2)
         this->root_ = n1;
}

/*
---------------------------------------------------
End implementations for the BinarySearchTree class.
---------------------------------------------------
*/

// A free helper function to recursively delete all nodes in the BST.
template<typename Key, typename Value>
void clearHelper(Node<Key, Value>* node) {
    if(node == nullptr)
         return;
    clearHelper(node->getLeft());
    clearHelper(node->getRight());
    delete node;
}

// include print function (assumed to be in a separate file)
#include "print_bst.h"

#endif
