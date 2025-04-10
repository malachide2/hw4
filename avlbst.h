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
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
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
    virtual void insert(const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void leftRotation(AVLNode<Key, Value>* startingNode);
    void rightRotation(AVLNode<Key, Value>* startingNode);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item) {
  // TODO
  AVLNode<Key, Value>* currNode = static_cast<AVLNode<Key, Value>*>(internalFind(key));
    if (currNode == nullptr) {
        return; // Key not found
    }

    AVLNode<Key, Value>* parentNode = currNode->getParent();
    bool balanceChanged = false;

    // Case 1: Node has two children
    if (currNode->getLeft() != nullptr && currNode->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(predecessor(currNode));
        nodeSwap(currNode, pred);
        parentNode = currNode->getParent(); // Update parent after swap
    }

    // Case 2: Node has one or zero children
    AVLNode<Key, Value>* child = (currNode->getLeft() != nullptr) ? currNode->getLeft() : currNode->getRight();
    if (currNode == this->root_) {
        this->root_ = child; // Update root if necessary
        if (child != nullptr) {
            child->setParent(nullptr);
        }
    } else {
        if (currNode == parentNode->getLeft()) {
            parentNode->setLeft(child);
            if (child != nullptr) {
                child->setParent(parentNode);
            }
            balanceChanged = true;
        } else {
            parentNode->setRight(child);
            if (child != nullptr) {
                child->setParent(parentNode);
            }
            balanceChanged = true;
        }
    }

    delete currNode;

    // Step 3: Update balances and perform rotations
    AVLNode<Key, Value>* curr = parentNode;
    while (curr != nullptr && balanceChanged) {
        int8_t oldBalance = curr->getBalance();

        if (child == curr->getLeft()) {
            curr->updateBalance(1);
        } else {
            curr->updateBalance(-1);
        }

        if (curr->getBalance() == 2) {
            AVLNode<Key, Value>* rightChild = curr->getRight();
            if (rightChild->getBalance() == -1) {
                rightRotation(rightChild);
            }
            leftRotation(curr);
        } else if (curr->getBalance() == -2) {
            AVLNode<Key, Value>* leftChild = curr->getLeft();
            if (leftChild->getBalance() == 1) {
                leftRotation(leftChild);
            }
            rightRotation(curr);
        }

        if (curr->getBalance() != 0 || oldBalance == 0) {
            break; // Stop if balance is restored or no further changes are needed
        }

        child = curr;
        curr = curr->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
  // TODO
  AVLNode<Key, Value>* currNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (currNode == nullptr) {
        return; // Key not found
    }

    AVLNode<Key, Value>* parentNode = currNode->getParent();
    bool isLeftChild = (parentNode != nullptr && currNode == parentNode->getLeft());

    // Case 1: Node has two children
    if (currNode->getLeft() != nullptr && currNode->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(currNode));
        nodeSwap(currNode, pred);
        parentNode = currNode->getParent();
        isLeftChild = (parentNode != nullptr && currNode == parentNode->getLeft());
    }

    // Case 2: Node has one or zero children
    AVLNode<Key, Value>* child = (currNode->getLeft() != nullptr) ? currNode->getLeft() : currNode->getRight();
    if (currNode == this->root_) {
        this->root_ = child; // Update root if necessary
        if (child != nullptr) {
            child->setParent(nullptr);
        }
    } else {
        if (isLeftChild) {
            parentNode->setLeft(child);
        } else {
            parentNode->setRight(child);
        }
        if (child != nullptr) {
            child->setParent(parentNode);
        }
    }

    delete currNode;

    // Step 3: Update balances and perform rotations
    AVLNode<Key, Value>* curr = parentNode;
    while (curr != nullptr) {
        if (isLeftChild) {
            curr->updateBalance(1); // Removing from left increases balance
        } else {
            curr->updateBalance(-1); // Removing from right decreases balance
        }

        if (curr->getBalance() == 2) {
            AVLNode<Key, Value>* rightChild = curr->getRight();
            if (rightChild->getBalance() == -1) {
                rightRotation(rightChild);
            }
            leftRotation(curr);
        } else if (curr->getBalance() == -2) {
            AVLNode<Key, Value>* leftChild = curr->getLeft();
            if (leftChild->getBalance() == 1) {
                leftRotation(leftChild);
            }
            rightRotation(curr);
        }

        if (curr->getBalance() != 0) {
            break; // Stop if balance is restored
        }

        isLeftChild = (curr->getParent() != nullptr && curr == curr->getParent()->getLeft());
        curr = curr->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftRotation(AVLNode<Key, Value>* startingNode) {
  AVLNode<Key, Value>* rightChild = startingNode->getRight();
    rightChild->setParent(startingNode->getParent());

    if (startingNode == this->root_) {
        this->root_ = rightChild; // Update root if necessary
    } else if (startingNode == startingNode->getParent()->getLeft()) {
        startingNode->getParent()->setLeft(rightChild);
    } else {
        startingNode->getParent()->setRight(rightChild);
    }

    startingNode->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr) {
        rightChild->getLeft()->setParent(startingNode);
    }

    rightChild->setLeft(startingNode);
    startingNode->setParent(rightChild);

    // Update balances
    rightChild->setBalance(rightChild->getBalance() - 1);
    startingNode->setBalance(startingNode->getBalance() - 2);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotation(AVLNode<Key, Value>* startingNode) {
  AVLNode<Key, Value>* leftChild = startingNode->getLeft();
    leftChild->setParent(startingNode->getParent());

    if (startingNode == this->root_) {
        this->root_ = leftChild; // Update root if necessary
    } else if (startingNode == startingNode->getParent()->getLeft()) {
        startingNode->getParent()->setLeft(leftChild);
    } else {
        startingNode->getParent()->setRight(leftChild);
    }

    startingNode->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr) {
        leftChild->getRight()->setParent(startingNode);
    }

    leftChild->setRight(startingNode);
    startingNode->setParent(leftChild);

    // Update balances
    leftChild->setBalance(leftChild->getBalance() + 1);
    startingNode->setBalance(startingNode->getBalance() + 2);
}


#endif
