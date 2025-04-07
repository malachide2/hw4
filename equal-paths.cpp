#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
#include <algorithm>
using namespace std;


// You may add any prototypes of helper functions here
int dfs(Node* node) {
  if (node == nullptr)
    return 0;
    
  int leftDepth = 0;
  int rightDepth = 0;
  if (node->left != nullptr) {
    leftDepth = dfs(node->left);
  }
  if (node->right != nullptr) {
    rightDepth = dfs(node->right);
  }
  return std::max(leftDepth, rightDepth) + 1;
}

bool equalPaths(Node * root) {
  // Add your code below
  return (abs(dfs(root->left) - dfs(root->right)) <= 1);
}