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
  Node<Key, Value>* originalNode = internalFind(new_item.first);
  if (originalNode != nullptr) {
    originalNode->setValue(new_item.second);
    return;
  }

  BinarySearchTree<Key, Value>::insert(new_item);
  Node<Key, Value>* insertedNode = internalFind(new_item.first);
  AVLNode<Key, Value>* currNode = static_cast<AVLNode<Key, Value>*>(insertedNode);
  AVLNode<Key, Value>* parentNode = currNode->getParent();
  while (parentNode != nullptr) {
    if (currNode == parentNode->getLeft()) {
      parentNode->updateBalance(-1);
    }
    else {
      parentNode->updateBalance(1);
    }

    if (parentNode->getBalance() == 0) {
      break;
    }
    else if (parentNode->getBalance() == -2) {
      if (currNode->getBalance() == 1) {
        // Left-Right Zig-Zag
        leftRotation(currNode);
      }
      rightRotation(parentNode);
      break;
    }
    else if (parentNode->getBalance() == 2) {
      // Right-heavy
      if (currNode->getBalance() == -1) {
        // Right-Left Zig-Zag
        rightRotation(currNode);
      }
      leftRotation(parentNode);
      break;
    }

    currNode = parentNode;
    parentNode = parentNode->getParent();
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
  // TODO
  AVLNode<Key, Value>* currNode = static_cast<AVLNode<Key, Value>*>(internalFind(key));
  if (currNode == nullptr)
    return;

  AVLNode<Key, Value>* parentNode = currNode->getParent();
  bool isLeftChild = (parentNode != nullptr && currNode == parentNode->getLeft());

  BinarySearchTree<Key, Value>::remove(key);

  while (parentNode != nullptr) {
    if (isLeftChild) {
      parentNode->updateBalance(1);
    } else {
      parentNode->updateBalance(-1);
    }

    if (parentNode->getBalance() == 2) {
      AVLNode<Key, Value>* rightChild = parentNode->getRight();
      if (rightChild->getBalance() == -1) {
        // Right-Left Zig-Zag
        rightRotation(rightChild);
      }
      leftRotation(parentNode);
    } else if (parentNode->getBalance() == -2) {
      AVLNode<Key, Value>* leftChild = parentNode->getLeft();
      if (leftChild->getBalance() == 1) {
          // Left-Right Zig-Zag
          leftRotation(leftChild);
      }
      rightRotation(parentNode);
    }

    if (parentNode->getBalance() == 0)
      break;

    isLeftChild = (parentNode->getParent() != nullptr && parentNode == parentNode->getParent()->getLeft());
    parentNode = parentNode->getParent();
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
  AVLNode<Key, Value>* leftChild = startingNode->getLeft();
  leftChild->setParent(startingNode->getParent());
  startingNode->setLeft(leftChild->getRight());
  leftChild->setRight(startingNode);
  startingNode->setParent(leftChild);
  leftChild->setBalance(leftChild->getBalance() + 1);
  startingNode->setBalance(startingNode->getBalance() + 2);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotation(AVLNode<Key, Value>* startingNode) {
  AVLNode<Key, Value>* rightChild = startingNode->getRight();
  rightChild->setParent(startingNode->getParent());
  startingNode->setRight(rightChild->getLeft());
  rightChild->setLeft(startingNode);
  startingNode->setParent(rightChild);
  rightChild->setBalance(rightChild->getBalance() - 1);
  startingNode->setBalance(startingNode->getBalance() - 2);
}


#endif
