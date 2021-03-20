/**
* @Authors: Cassandra Olfert, Devin Schafthuizen, Kate Ye
* @Class - CPSC 3620: Data structures and algorithms
* @Data Structure Performance and Evaluation - AVL tress
* @Date: April 12,2021
*/
#include <iostream>
#include <fstream>
#include <chrono>

class Node {
   public:
  Node (int k, int h) : key(k), height(h) {}
  int key;
  Node* left = NULL;
  Node* right = NULL;
  int height;
};

int height(Node* n) {
  if (n == NULL)
    return 0;
  return n->height;
}

int max(int a, int b) {
  return (a > b) ? a : b; // returns the larger key
}

Node* rightRotate(Node* n) {
  Node* x = n->left;
  Node* temp = x->right;
  x->right = n;
  n->left = temp;
  n->height = max(height(n->left), height(n->right)) + 1;
  x->height = max(height(x->left), height(x->right)) + 1;
  return x;
}

Node* leftRotate(Node* n ) {
  Node* x = n->right;
  Node* temp = x->left;
  x->left = n;
  n->right = temp;
  n->height = max(height(n->left), height(n->right)) + 1;
  x->height = max(height(x->left), height(x->right)) + 1;
  return x;
}

int getBalanceFactor(Node* N) {
  return height(N->left) - height(N->right);
}

Node* rebalnce(Node* node, int key) {
  // Rebalnce the tree after adding the node
  node->height = 1 + max(height(node->left), height(node->right));
  int balanceFactor = getBalanceFactor(node);
  if (balanceFactor > 1) {
    if (key < node->left->key) {                 // Right rotate
      return rightRotate(node);
    } else if (key > node->left->key) {         // Left Right rotate
      node->left = leftRotate(node->left);
      return rightRotate(node);
    }
  }
  if (balanceFactor < -1) {
    if (key > node->right->key) {               // Left Rotate
      return leftRotate(node);
    } else if (key < node->right->key) {        // Right Left Rotate
      node->right = rightRotate(node->right);
      return leftRotate(node);
    }
  }
  return node;
}

Node* addNode(Node* node, int key) {
  // add the node at an empty spot, or continue searching down the tree
  if (node == NULL) {
    Node* n = new Node(key, 1);
    node = rebalnce(n, key);
  }
  if (key < node->key)
    node->left = addNode(node->left, key);
  else if (key > node->key)
    node->right = addNode(node->right, key);
  else
    return node;

  node = rebalnce(node, key);
  return node;
}

// Find the bottom of the tree and point to that
Node* bottomNode(Node *node) {
  while (node->left != NULL)
    node = node->left;
  return node;
}

Node* removeNode(Node* node, int key) {
  if (node == NULL)
    return node;
  if (key < node->key)
    node->left = removeNode(node->left, key);
  else if (key > node->key)
    node->right = removeNode(node->right, key);
  else {
    if ((node->left == NULL) || (node->right == NULL)) {
      Node* temp = node->left ? node->left : node->right;
      if (temp == NULL) {
        temp = node;
        node = NULL;
      } else
        *node = *temp;
      free(temp);
    } else {
      Node *temp = bottomNode(node->right);
      node->key = temp->key;
      node->right = removeNode(node->right, temp->key);
    }
  }

  //node = rebalnce(node, 1);
  return node;
}

void displayTree(Node* root, std::string indent, bool last) {
  if (root != nullptr) {
    std::cout << indent;
    if (last) {
      std::cout << "R----";
      indent += "   ";
    } else {
      std::cout << "L----";
      indent += "|  ";
    }
    std::cout << root->key << std::endl;
    displayTree(root->left, indent, false);
    displayTree(root->right, indent, true);
  }
}

Node* search(Node* node, int key) {
  if (node == NULL) {
    std::cout << "The node " << key << " was not found" << std::endl;
    return nullptr;
  }
  else if (key == node->key) {
    std::cout << "Node " << key << " was found!" << std::endl;
    return node;
  }
  else if (key < node->key)
    node->left = search(node->left, key);
  else if (key > node->key)
    node->right = search(node->right, key);
  return node;
}

int main() {
  std::ifstream input;
  int temp;

  // Get starting timepoint
  auto start = std::chrono::high_resolution_clock::now();

  input.open ("input.txt");
  Node *root = NULL;
  std::cout << "Reading from file " << std::endl;
  while(!input.eof()) {
    input >> temp;
    // Get starting timepoint per addNode
    auto start3 = std::chrono::high_resolution_clock::now();
    root = addNode(root, temp);
    // Get ending timepoint
    auto stop3 = std::chrono::high_resolution_clock::now();
    // Get time taken and print it out per addNode call
    auto duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop3 - start3);
    std::cout << "Time taken per addNode function while reading from file: " << duration3.count() << " nanoseconds" << std::endl;
  }
  input.close();

  // Get ending timepoint
  auto stop = std::chrono::high_resolution_clock::now();

  // Get time taken and print it out
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
  std::cout << "Total time taken by addNode function while reading from file: " << duration.count() << " nanoseconds" << std::endl;

  // Get starting timepoint for displayTree
  auto start2 = std::chrono::high_resolution_clock::now();

  displayTree(root, "", true);

  // Testing search functions
  std::cout << std::endl << std::endl;
  root = search(root,5);
  root = search(root,20);
  root = search(root,15);
  root = search(root,8);
  root = search(root,1000);

  std::cout << "After removing a node.." << std::endl;
  root = removeNode(root, 8);
  root = search(root,8);
  displayTree(root, "", true);

  free(root);

  // Get ending timepoint
  auto stop2 = std::chrono::high_resolution_clock::now();

  // Get time taken for displayTree and print it out
  auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop2 - start2);
  std::cout << "Time taken by displayTree function: " << duration2.count() << " nanoseconds" << std::endl;
}
