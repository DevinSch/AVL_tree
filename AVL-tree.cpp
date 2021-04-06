/**
* @Authors: Cassandra Olfert, Devin Schafthuizen, Kate Ye
* @Class - CPSC 3620: Data structures and algorithms
* @Data Structure Performance and Evaluation - AVL tress
* @Date: April 12,2021
*/
#include <iostream>
#include <fstream>
#include <chrono>

int MemoryUsed = 0; // Used to track all memory curently being used

class Node {
   public:
  Node (int k, int h) : key(k), left(NULL), right(NULL), height(h) {}
  int key;
  Node* left;
  Node* right;
  int height;
};

// Override the new operator to keep track of memory being used when new is called
void* operator new(size_t size) {
  std::cout << "Allocating " << size << " bytes" << std::endl;
  MemoryUsed += size;
  return malloc(size);
}

// Overside the delete operator to keep track of memory being freed when delete is called
void operator delete(void* memory, size_t size) {
  std::cout << "Deallocating " << size << " bytes" << std::endl;
  MemoryUsed -= size;
  free(memory);
}


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

Node* rebalance(Node* node, int key) {
  // Rebalance the tree after adding the node
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
    node = rebalance(n, key);
  }
  if (key < node->key)
    node->left = addNode(node->left, key);
  else if (key > node->key)
    node->right = addNode(node->right, key);
  else
    return node;

  node = rebalance(node, key);
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
      delete(temp);
    } else {
      Node *temp = bottomNode(node->right);
      node->key = temp->key;
      node->right = removeNode(node->right, temp->key);
    }
  }

  //node = rebalance(node, 1);
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

// Free all memory used in a given tree, should be used to complete deallocating
// all the memory used by that tree from the root node given
void destroy_tree(Node* node) {
  if (node != nullptr) {
    if (node->left!=NULL)
      destroy_tree(node->left);
    if (node->right!=NULL)
      destroy_tree(node->right);
  }
  delete(node);
}

// experiment 1: inserts all elements from file, searches for 10 elements, then deletes 5 elements
void experiment1(std::ifstream& inputFile) {
  int temp;

  Node *root = NULL;
  std::cout << std::endl << "Reading from file to insert elements." << std::endl;

  std::chrono::high_resolution_clock::time_point start1 = std::chrono::high_resolution_clock::now();

  while(!inputFile.eof()) {
    inputFile >> temp;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    root = addNode(root, temp);

    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    std::cout << "Time taken per addNode function while reading from file: " << duration.count() << " nanoseconds" << std::endl;
  }

  std::chrono::high_resolution_clock::time_point stop1 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop1 - start1);
  std::cout << "Total time taken to add all elements to tree from file: " << duration1.count() << " nanoseconds" << std::endl;

  displayTree(root, "", true);

 std::cout << std::endl << std::endl;

  // Searching for 10 nodes
   std::cout << "Searching for 10 elements: 5, 20, 156, 1, 78, 1000, 15, 86, 14, 6, 99." << std::endl;

  std::chrono::high_resolution_clock::time_point start2 = std::chrono::high_resolution_clock::now();

  root = search(root,5);
  root = search(root,20);
  root = search(root,156);
  root = search(root,1);
  root = search(root,78);
  root = search(root,1000);
  root = search(root,15);
  root = search(root,86);
  root = search(root,14);
  root = search(root,6);
  root = search(root,99);

  std::chrono::high_resolution_clock::time_point stop2 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop2 - start2);
  std::cout << "Total time taken to search for 10 elements: " << duration2.count() << " nanoseconds" << std::endl;

  // Deleting 5 nodes
  std::cout << "Deleting 5 elements: 2, 62, 14, 283, 99; if they exist." << std::endl;

  std::chrono::high_resolution_clock::time_point start3 = std::chrono::high_resolution_clock::now();

  root = removeNode(root, 2);
  root = removeNode(root, 62);
  root = removeNode(root, 14);
  root = removeNode(root, 283);
  root = removeNode(root, 99);

  std::chrono::high_resolution_clock::time_point stop3 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop3 - start3);
  std::cout << "Total time taken to delete 5 elements: " << duration3.count() << " nanoseconds" << std::endl;

  displayTree(root, "", true);

  std::cout << "Total memory used: " << MemoryUsed << " bytes." << std::endl;

  destroy_tree(root);

  std::cout << "Memory after free: " << MemoryUsed << " bytes." << std::endl;

  std::cout << std::endl << std::endl;
}

// experiment2: inserts all elements, deletes 10 elements, then searches for 15 elements
void experiment2(std::ifstream& inputFile) {
  int temp;

  Node *root = NULL;
  std::cout << std::endl << "Reading from file to insert elements." << std::endl;

  std::chrono::high_resolution_clock::time_point start1 = std::chrono::high_resolution_clock::now();

  while(!inputFile.eof()) {
    inputFile >> temp;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    root = addNode(root, temp);

    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    std::cout << "Time taken per addNode function while reading from file: " << duration.count() << " nanoseconds" << std::endl;
  }

  std::chrono::high_resolution_clock::time_point stop1 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration1 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop1 - start1);
  std::cout << "Total time taken to add all elements to tree from file: " << duration1.count() << " nanoseconds" << std::endl;

  displayTree(root, "", true);

  // Deleting nodes
  std::cout << "Removing 10 elements: 78, 1, 14, 66, 100, 30, 283, 4, 56, 32; if they exist." << std::endl;

  std::chrono::high_resolution_clock::time_point start2 = std::chrono::high_resolution_clock::now();

  root = removeNode(root, 78);
  root = removeNode(root, 1);
  root = removeNode(root, 14);
  root = removeNode(root, 66);
  root = removeNode(root, 100);
  root = removeNode(root, 30);
  root = removeNode(root, 283);
  root = removeNode(root, 4);
  root = removeNode(root, 56);
  root = removeNode(root, 32);


  std::chrono::high_resolution_clock::time_point stop2 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop2 - start2);
  std::cout << "Total time taken to delete 10 elements: " << duration2.count() << " nanoseconds" << std::endl;

  displayTree(root, "", true);

  std::cout << std::endl << std::endl;

  // Searching for 15 nodes
  std::cout << "Searching for 15 elements: 5, 20, 156, 1, 78, 1000, 90, 204, 24, 2, 42, 99, 25, 12, 873." << std::endl;

  std::chrono::high_resolution_clock::time_point start3 = std::chrono::high_resolution_clock::now();

  root = search(root,5);
  root = search(root,20);
  root = search(root,156);
  root = search(root,1);
  root = search(root,78);
  root = search(root,1000);
  root = search(root,90);
  root = search(root,204);
  root = search(root,24);
  root = search(root,2);
  root = search(root,42);
  root = search(root,99);
  root = search(root,25);
  root = search(root,12);
  root = search(root,873);

  std::chrono::high_resolution_clock::time_point stop3 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop3 - start3);
  std::cout << "Total time taken to search for 15 elements: " << duration3.count() << " nanoseconds" << std::endl;

  std::cout << "Total memory used: " << MemoryUsed << " bytes." << std::endl;

  destroy_tree(root);

  std::cout << "Memory after free: " << MemoryUsed << " bytes." << std::endl;

  std::cout << std::endl << std::endl;
}

int main() {
  // std::ifstream input;
  // int temp;

  // // Get starting timepoint
  // std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

  // input.open ("input2.txt");
  // Node *root = NULL;
  // std::cout << "TEST: Reading from file input2.txt" << std::endl;
  // while(!input.eof()) {
  //   input >> temp;
  //   // Get starting timepoint per addNode
  //   std::chrono::high_resolution_clock::time_point start3 = std::chrono::high_resolution_clock::now();
  //   root = addNode(root, temp);
  //   // Get ending timepoint
  //   std::chrono::high_resolution_clock::time_point stop3 = std::chrono::high_resolution_clock::now();
  //   // Get time taken and print it out per addNode call
  //   std::chrono::nanoseconds duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop3 - start3);
  //   std::cout << "Time taken per addNode function while reading from file: " << duration3.count() << " nanoseconds" << std::endl;
  // }
  // input.close();

  // // Get ending timepoint
  // std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();

  // // Get time taken and print it out
  // std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
  // std::cout << "Total time taken by addNode function while reading from file: " << duration.count() << " nanoseconds" << std::endl;

  // // Get starting timepoint for displayTree
  // std::chrono::high_resolution_clock::time_point start2 = std::chrono::high_resolution_clock::now();

  // displayTree(root, "", true);

  // // Testing search functions
  // std::cout << std::endl << std::endl;
  // root = search(root,5);
  // root = search(root,20);
  // root = search(root,15);
  // root = search(root,8);
  // root = search(root,1000);

  // std::cout << "After removing a node.." << std::endl;
  // root = removeNode(root, 8);
  // root = search(root,8);
  // displayTree(root, "", true);
  // std::cout << "Total memory used " << MemoryUsed << " bytes " << std::endl;
  // destroy_tree(root);
  // std::cout << "Memory after free " << MemoryUsed << " bytes " << std::endl;

  // // Get ending timepoint
  // std::chrono::high_resolution_clock::time_point stop2 = std::chrono::high_resolution_clock::now();

  // // Get time taken for displayTree and print it out
  // std::chrono::nanoseconds duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop2 - start2);
  // std::cout << "Time taken by displayTree function: " << duration2.count() << " nanoseconds" << std::endl;

  std::ifstream inputFile;
  std::ifstream inputFile2;
  std::ifstream inputFile3;
  std::ifstream inputFile4;
  std::ifstream inputFile5;
  std::ifstream inputFile6;
  std::ifstream inputFile7;
  std::ifstream inputFile8;
  std::ifstream inputFile9;
  std::ifstream inputFile10;
  std::ifstream inputFile11;
  std::ifstream inputFile12;

  // run experiment1 on 6 unsorted lists

  //Experiment 1.1
  std::chrono::high_resolution_clock::time_point start_Exp1_1 = std::chrono::high_resolution_clock::now();
  inputFile.open("input_15_unsorted.txt");
  std::cout << "Experiment 1.1: 15 unsorted elements" << std::endl;
  experiment1(inputFile);
  inputFile.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_1 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_1 - start_Exp1_1);
  std::cout << "Total Time taken by Experiment 1.1: " << duration_Exp1_1.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.2
  std::chrono::high_resolution_clock::time_point start_Exp1_2 = std::chrono::high_resolution_clock::now();
  inputFile2.open("input_30_unsorted.txt");
  std::cout << "Experiment 1.2: 30 unsorted elements" << std::endl;
  experiment1(inputFile2);
  inputFile2.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_2 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_2 - start_Exp1_2);
  std::cout << "Total Time taken by Experiment 1.2: " << duration_Exp1_2.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.3
  std::chrono::high_resolution_clock::time_point start_Exp1_3 = std::chrono::high_resolution_clock::now();
  inputFile3.open("input_45_unsorted.txt");
  std::cout << "Experiment 1.3: 45 unsorted elements" << std::endl;
  experiment1(inputFile3);
  inputFile3.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_3 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_3 - start_Exp1_3);
  std::cout << "Total Time taken by Experiment 1.3: " << duration_Exp1_3.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.4
  std::chrono::high_resolution_clock::time_point start_Exp1_4 = std::chrono::high_resolution_clock::now();
  inputFile4.open("input_60_unsorted.txt");
  std::cout << "Experiment 1.4: 60 unsorted elements" << std::endl;
  experiment1(inputFile4);
  inputFile4.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_4 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_4 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_4 - start_Exp1_4);
  std::cout << "Total Time taken by Experiment 1.4: " << duration_Exp1_4.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.5
  std::chrono::high_resolution_clock::time_point start_Exp1_5 = std::chrono::high_resolution_clock::now();
  inputFile5.open("input_75_unsorted.txt");
  std::cout << "Experiment 1.5: 75 unsorted elements" << std::endl;
  experiment1(inputFile5);
  inputFile5.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_5 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_5 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_5 - start_Exp1_5);
  std::cout << "Total Time taken by Experiment 1.5: " << duration_Exp1_5.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.6
  std::chrono::high_resolution_clock::time_point start_Exp1_6 = std::chrono::high_resolution_clock::now();
  inputFile6.open("input_90_unsorted.txt");
  std::cout << "Experiment 1.6: 90 unsorted elements" << std::endl;
  experiment1(inputFile6);
  inputFile6.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_6 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_6 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_6 - start_Exp1_6);
  std::cout << "Total Time taken by Experiment 1.6: " << duration_Exp1_6.count() << " nanoseconds" << std::endl << std::endl;

  // run experiment1 on 6 sorted lists

  //Experiment 1.7
  std::chrono::high_resolution_clock::time_point start_Exp1_7 = std::chrono::high_resolution_clock::now();
  inputFile7.open("input_15_sorted.txt");
  std::cout << "Experiment 1.7: 15 sorted elements" << std::endl;
  experiment1(inputFile7);
  inputFile7.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_7 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_7 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_7 - start_Exp1_7);
  std::cout << "Total Time taken by Experiment 1.7: " << duration_Exp1_7.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.8
  std::chrono::high_resolution_clock::time_point start_Exp1_8 = std::chrono::high_resolution_clock::now();
  inputFile8.open("input_30_sorted.txt");
  std::cout << "Experiment 1.8: 30 sorted elements" << std::endl;
  experiment1(inputFile8);
  inputFile8.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_8 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_8 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_8 - start_Exp1_8);
  std::cout << "Total Time taken by Experiment 1.8: " << duration_Exp1_8.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.9
  std::chrono::high_resolution_clock::time_point start_Exp1_9 = std::chrono::high_resolution_clock::now();
  inputFile9.open("input_45_sorted.txt");
  std::cout << "Experiment 1.9: 45 sorted elements" << std::endl;
  experiment1(inputFile9);
  inputFile9.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_9 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_9 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_9 - start_Exp1_9);
  std::cout << "Total Time taken by Experiment 1.9: " << duration_Exp1_9.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.10
  std::chrono::high_resolution_clock::time_point start_Exp1_10 = std::chrono::high_resolution_clock::now();
  inputFile10.open("input_60_sorted.txt");
  std::cout << "Experiment 1.10: 60 sorted elements" << std::endl;
  experiment1(inputFile10);
  inputFile10.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_10 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_10 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_10 - start_Exp1_10);
  std::cout << "Total Time taken by Experiment 1.10: " << duration_Exp1_10.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.11
  std::chrono::high_resolution_clock::time_point start_Exp1_11 = std::chrono::high_resolution_clock::now();
  inputFile11.open("input_75_sorted.txt");
  std::cout << "Experiment 1.11: 75 sorted elements" << std::endl;
  experiment1(inputFile11);
  inputFile11.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_11 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_11 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_11 - start_Exp1_11);
  std::cout << "Total Time taken by Experiment 1.11: " << duration_Exp1_11.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 1.12
  std::chrono::high_resolution_clock::time_point start_Exp1_12 = std::chrono::high_resolution_clock::now();
  inputFile12.open("input_90_sorted.txt");
  std::cout << "Experiment 1.12: 90 sorted elements" << std::endl;
  experiment1(inputFile12);
  inputFile12.close();
  std::chrono::high_resolution_clock::time_point stop_Exp1_12 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp1_12 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp1_12 - start_Exp1_12);
  std::cout << "Total Time taken by Experiment 1.12: " << duration_Exp1_12.count() << " nanoseconds" << std::endl << std::endl;

  // run experiment2 on 6 unsorted lists

  //Experiment 2.1
  std::chrono::high_resolution_clock::time_point start_Exp2_1 = std::chrono::high_resolution_clock::now();
  inputFile.open("input_15_unsorted.txt");
  std::cout << "Experiment 2.1: 15 unsorted elements" << std::endl;
  experiment2(inputFile);
  inputFile.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_1 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_1 - start_Exp2_1);
  std::cout << "Total Time taken by Experiment 2.1: " << duration_Exp2_1.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.2
  std::chrono::high_resolution_clock::time_point start_Exp2_2 = std::chrono::high_resolution_clock::now();
  inputFile2.open("input_30_unsorted.txt");
  std::cout << "Experiment 2.2: 30 unsorted elements" << std::endl;
  experiment2(inputFile2);
  inputFile2.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_2 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_2 - start_Exp2_2);
  std::cout << "Total Time taken by Experiment 2.2: " << duration_Exp2_2.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.3
  std::chrono::high_resolution_clock::time_point start_Exp2_3 = std::chrono::high_resolution_clock::now();
  inputFile3.open("input_45_unsorted.txt");
  std::cout << "Experiment 2.3: 45 unsorted elements" << std::endl;
  experiment2(inputFile3);
  inputFile3.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_3 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_3 - start_Exp2_3);
  std::cout << "Total Time taken by Experiment 2.3: " << duration_Exp2_3.count() << " nanoseconds" << std::endl << std::endl;

  // Experiment 2.4
  std::chrono::high_resolution_clock::time_point start_Exp2_4 = std::chrono::high_resolution_clock::now();
  inputFile4.open("input_60_unsorted.txt");
  std::cout << "Experiment 2.4: 60 unsorted elements" << std::endl;
  experiment2(inputFile4);
  inputFile4.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_4 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_4 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_4 - start_Exp2_4);
  std::cout << "Total Time taken by Experiment 2.4: " << duration_Exp2_4.count() << " nanoseconds" << std::endl << std::endl;

  // Experiment 2.5
  std::chrono::high_resolution_clock::time_point start_Exp2_5 = std::chrono::high_resolution_clock::now();
  inputFile5.open("input_75_unsorted.txt");
  std::cout << "Experiment 2.5: 75 unsorted elements" << std::endl;
  experiment2(inputFile5);
  inputFile5.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_5 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_5 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_5 - start_Exp2_5);
  std::cout << "Total Time taken by Experiment 2.5: " << duration_Exp2_5.count() << " nanoseconds" << std::endl << std::endl;

  // Experiment 2.6
  std::chrono::high_resolution_clock::time_point start_Exp2_6 = std::chrono::high_resolution_clock::now();
  inputFile6.open("input_90_unsorted.txt");
  std::cout << "Experiment 2.6: 90 unsorted elements" << std::endl;
  experiment2(inputFile6);
  inputFile6.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_6 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_6 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_6 - start_Exp2_6);
  std::cout << "Total Time taken by Experiment 2.6: " << duration_Exp2_6.count() << " nanoseconds" << std::endl << std::endl;

  // run experiment2 on 6 sorted lists

  //Experiment 2.7
  std::chrono::high_resolution_clock::time_point start_Exp2_7 = std::chrono::high_resolution_clock::now();
  inputFile7.open("input_15_sorted.txt");
  std::cout << "Experiment 2.7: 15 sorted elements" << std::endl;
  experiment2(inputFile7);
  inputFile7.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_7 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_7 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_7 - start_Exp2_7);
  std::cout << "Total Time taken by Experiment 2.7: " << duration_Exp2_7.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.8
  std::chrono::high_resolution_clock::time_point start_Exp2_8 = std::chrono::high_resolution_clock::now();
  inputFile8.open("input_30_sorted.txt");
  std::cout << "Experiment 2.8: 30 sorted elements" << std::endl;
  experiment2(inputFile8);
  inputFile8.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_8 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_8 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_8 - start_Exp2_8);
  std::cout << "Total Time taken by Experiment 2.8: " << duration_Exp2_8.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.9
  std::chrono::high_resolution_clock::time_point start_Exp2_9 = std::chrono::high_resolution_clock::now();
  inputFile9.open("input_45_sorted.txt");
  std::cout << "Experiment 2.9: 45 sorted elements" << std::endl;
  experiment2(inputFile9);
  inputFile9.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_9 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_9 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_9 - start_Exp2_9);
  std::cout << "Total Time taken by Experiment 2.9: " << duration_Exp2_9.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.10
  std::chrono::high_resolution_clock::time_point start_Exp2_10 = std::chrono::high_resolution_clock::now();
  inputFile10.open("input_60_sorted.txt");
  std::cout << "Experiment 2.10: 60 sorted elements" << std::endl;
  experiment2(inputFile10);
  inputFile10.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_10 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_10 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_10 - start_Exp2_10);
  std::cout << "Total Time taken by Experiment 2.10: " << duration_Exp2_10.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.11
  std::chrono::high_resolution_clock::time_point start_Exp2_11 = std::chrono::high_resolution_clock::now();
  inputFile11.open("input_75_sorted.txt");
  std::cout << "Experiment 2.11: 75 sorted elements" << std::endl;
  experiment2(inputFile11);
  inputFile11.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_11 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_11 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_11 - start_Exp2_11);
  std::cout << "Total Time taken by Experiment 2.11: " << duration_Exp2_11.count() << " nanoseconds" << std::endl << std::endl;

  //Experiment 2.12
  std::chrono::high_resolution_clock::time_point start_Exp2_12 = std::chrono::high_resolution_clock::now();
  inputFile12.open("input_90_sorted.txt");
  std::cout << "Experiment 2.12: 90 sorted elements" << std::endl;
  experiment2(inputFile12);
  inputFile12.close();
  std::chrono::high_resolution_clock::time_point stop_Exp2_12 = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds duration_Exp2_12 = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_Exp2_12 - start_Exp2_12);
  std::cout << "Total Time taken by Experiment 2.12: " << duration_Exp2_12.count() << " nanoseconds" << std::endl << std::endl;
}
