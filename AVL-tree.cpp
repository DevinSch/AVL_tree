#include <iostream>
#include <fstream>

class Node {
   public:
  Node (int k) : value(k) {}
  int value;
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
  return (a > b) ? a : b; // returns the larger value
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

Node* addNode(Node* node, int value) {
  // add the node at an empty spot, or continue searching down the tree
  if (node == NULL) {
    Node* n = new Node(value);
    return (n);
  }
  if (value < node->value)
    node->left = addNode(node->left, value);
  else if (value > node->value)
    node->right = addNode(node->right, value);
  else
    return node;

  // Rebalnce the tree after adding the node
  node->height = 1 + max(height(node->left), height(node->right));
  int balanceFactor = getBalanceFactor(node);
  if (balanceFactor > 1) {
    if (value < node->left->value) {                 // Right rotate
      return rightRotate(node);
    } else if (value > node->left->value) {         // Left Right rotate
      node->left = leftRotate(node->left);
      return rightRotate(node);
    }
  }
  if (balanceFactor < -1) {
    if (value > node->right->value) {               // Left Rotate
      return leftRotate(node);
    } else if (value < node->right->value) {        // Right Left Rotate
      node->right = rightRotate(node->right);
      return leftRotate(node);
    }
  }
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
    std::cout << root->value << std::endl;
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
