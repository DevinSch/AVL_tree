/**
* @Authors: Cassandra Olfert, Devin Schafthuizen, Kate Ye
* @Class - CPSC 3620: Data structures and algorithms
* @Data Structure Performance and Evaluation - AVL tress
* @Date: April 12,2021
*/
#include <iostream>
#include <fstream>

class Node {
   public:
  Node (int k) : Key(k) {}
  int Key;
  Node* left = NULL;
  Node* right = NULL;
  int height = 1;
};

int height(Node* n) {
  if (n == NULL)
    return 0;
  return n->height;
}

int max(int a, int b) {
  return (a > b) ? a : b; // returns the larger Key
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

Node* rebalnce(Node* node, int Key) {
  // Rebalnce the tree after adding the node
  node->height = 1 + max(height(node->left), height(node->right));
  int balanceFactor = getBalanceFactor(node);
  if (balanceFactor > 1) {
    if (Key < node->left->Key) {                 // Right rotate
      return rightRotate(node);
    } else if (Key > node->left->Key) {         // Left Right rotate
      node->left = leftRotate(node->left);
      return rightRotate(node);
    }
  }
  if (balanceFactor < -1) {
    if (Key > node->right->Key) {               // Left Rotate
      return leftRotate(node);
    } else if (Key < node->right->Key) {        // Right Left Rotate
      node->right = rightRotate(node->right);
      return leftRotate(node);
    }
  }
  return node;

}

Node* addNode(Node* node, int Key) {
  // add the node at an empty spot, or continue searching down the tree
  if (node == NULL) {
    Node* n = new Node(Key);
    return (n);
  }
  if (Key < node->Key)
    node->left = addNode(node->left, Key);
  else if (Key > node->Key)
    node->right = addNode(node->right, Key);
  else
    return node;

  node = rebalnce(node, Key);
  return node;
}


// Work in progress, Not currently working
Node* removeNode(Node* node, int Key) {
  if (node == NULL)
    return node;
  if (Key < node->Key)
    node->left = removeNode(node->left, Key);
  else if (Key > node->Key)
    node->right = removeNode(node->right, Key);
  else {
  }

  node = rebalnce(node, Key);
  return node;
}

void displayTree(Node *root, std::string indent, bool last) {
  if (root != nullptr) {
    std::cout << indent;
    if (last) {
      std::cout << "R----";
      indent += "   ";
    } else {
      std::cout << "L----";
      indent += "|  ";
    }
    std::cout << root->Key << std::endl;
    displayTree(root->left, indent, false);
    displayTree(root->right, indent, true);
  }
}

int main() {
  std::ifstream input;
  int temp;
  input.open ("input.txt");
  Node *root = NULL;
  std::cout << "Reading from file " << std::endl;
  while(!input.eof()) {
    input >> temp;
    root = addNode(root, temp);
  }
  input.close();
  displayTree(root, "", true);
}
