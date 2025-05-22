#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <cassert>

using namespace std;

// Node structure for the Leftist Tree
struct Node {
    int key;     // The value stored in the node
    int npl;     // Null Path Length - length of the shortest path from this node to a null child
    Node *left;  // Pointer to the left child
    Node *right; // Pointer to the right child

    Node(int k) : key(k), npl(0), left(nullptr), right(nullptr) {}
};

class LeftistTree {
private:
    // The root node of the leftist tree.
    Node *root;

    // Helper function to get NPL of a node (handles null)
    int getNPL(Node* node) {
        if (node == nullptr) {
            return -1;
        }
        return node->npl;
    }

    // Helper function to swap children of a node
    void swapChildren(Node* node) {
        if (node) {
            swap(node->left, node->right);
        }
    }

    // Merges two leftist heaps h1 and h2, returns the root of the merged heap
    // Time Complexity: O(log N), where N is the total number of nodes in both heaps.
    // This is because the path we traverse during merging is always the right path,
    // which has a maximum length proportional to the logarithm of the number of nodes.
    // Space complexity for storing the tree itself: O(N)
    Node* merge(Node* h1, Node* h2) {
        if (h1 == nullptr) return h2;
        if (h2 == nullptr) return h1;

        if (h1->key > h2->key) {
            swap(h1, h2);
        }

        h1->right = merge(h1->right, h2);

        if (getNPL(h1->left) < getNPL(h1->right)) {
            swapChildren(h1);
        }

        h1->npl = getNPL(h1->right) + 1;

        return h1;
    }

    // Helper for destructor: recursively delete nodes
    void destroyRecursive(Node* node) {
        if (node) {
            destroyRecursive(node->left);
            destroyRecursive(node->right);
            delete node;
        }
    }

    // Helper for pretty printing the tree structure
    void printTreeRecursive(Node* node, const string& prefix, bool isLeft) const {
        if (node == nullptr) {
            return;
        }

        cout << prefix;
        cout << (isLeft ? "├──L:" : "└──R:");
        cout << node->key << " (npl:" << node->npl << ")" << endl;

        printTreeRecursive(node->left, prefix + (isLeft ? "│   " : "    "), true);
        printTreeRecursive(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }


public:
    LeftistTree() : root(nullptr) {}

    ~LeftistTree() {
        destroyRecursive(root);
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    // Insert a new key into the tree
    void insert(int key) {
        Node* newNode = new Node(key);
        root = merge(root, newNode);
    }

    // Get the minimum key (root key) without removing it
    int getMin() const {
        if (isEmpty()) {
            throw runtime_error("Heap is empty!");
        }
        return root->key;
    }

    // Remove and return the minimum key
    int extractMin() {
        if (isEmpty()) {
            throw runtime_error("Heap is empty! Cannot extract min.");
        }

        int minKey = root->key;
        Node* oldRoot = root;
        root = merge(root->left, root->right);
        delete oldRoot;
        return minKey;
    }

    // Public interface to merge another LeftistTree into this one
    void mergeWith(LeftistTree& otherTree) {
        if (this == &otherTree) {
            return;
        }
        root = merge(this->root, otherTree.root);
        otherTree.root = nullptr;
    }

    void printTree() const {
        if (isEmpty()) {
            cout << "Tree is empty." << endl;
            return;
        }
        printTreeRecursive(root, "", false);
    }
};

void testLeftistTree() {
    cout << "Starting LeftistTree tests..." << endl;

    // Test 1: Basic insert and getMin
    LeftistTree lt1;
    lt1.insert(10);
    lt1.insert(5);
    lt1.insert(20);
    assert(lt1.getMin() == 5 && "Test 1 Failed: Basic insert and getMin");
    cout << "Test 1 Passed." << endl;

    // Test 2: ExtractMin
    assert(lt1.extractMin() == 5 && "Test 2 Failed: First extractMin");
    assert(lt1.getMin() == 10 && "Test 2 Failed: getMin after first extract");
    assert(lt1.extractMin() == 10 && "Test 2 Failed: Second extractMin");
    assert(lt1.getMin() == 20 && "Test 2 Failed: getMin after second extract");
    assert(lt1.extractMin() == 20 && "Test 2 Failed: Third extractMin");
    assert(lt1.isEmpty() && "Test 2 Failed: isEmpty after all extracts");
    cout << "Test 2 Passed." << endl;

    // Test 3: Insert multiple elements and extract all
    LeftistTree lt2;
    lt2.insert(15);
    lt2.insert(3);
    lt2.insert(8);
    lt2.insert(1);
    lt2.insert(12);
    assert(lt2.getMin() == 1 && "Test 3 Failed: getMin with multiple inserts");
    assert(lt2.extractMin() == 1 && "Test 3 Failed: extractMin 1");
    assert(lt2.extractMin() == 3 && "Test 3 Failed: extractMin 3");
    assert(lt2.extractMin() == 8 && "Test 3 Failed: extractMin 8");
    assert(lt2.extractMin() == 12 && "Test 3 Failed: extractMin 12");
    assert(lt2.extractMin() == 15 && "Test 3 Failed: extractMin 15");
    assert(lt2.isEmpty() && "Test 3 Failed: isEmpty after all extracts");
    cout << "Test 3 Passed." << endl;

    // Test 4: Merging trees
    LeftistTree lt3a, lt3b;
    lt3a.insert(10); lt3a.insert(20); lt3a.insert(5);
    lt3b.insert(15); lt3b.insert(8); lt3b.insert(25);
    lt3a.mergeWith(lt3b);
    assert(lt3b.isEmpty() && "Test 4 Failed: Merged tree B is not empty");
    assert(!lt3a.isEmpty() && "Test 4 Failed: Merged tree A is empty");

    vector<int> extracted_merged;
    while(!lt3a.isEmpty()) {
        extracted_merged.push_back(lt3a.extractMin());
    }
    vector<int> expected_merged = {5, 8, 10, 15, 20, 25};
    assert(extracted_merged == expected_merged && "Test 4 Failed: Merged tree elements incorrect");
    cout << "Test 4 Passed." << endl;

    // Test 5: Merging an empty tree
    LeftistTree lt4a, lt4b;
    lt4a.insert(100);
    lt4a.mergeWith(lt4b);
    assert(lt4a.getMin() == 100 && "Test 5 Failed: Merging with empty tree");
    assert(lt4b.isEmpty() && "Test 5 Failed: Empty tree changed after merge");
    assert(lt4a.extractMin() == 100 && "Test 5 Failed: Extract after merge with empty");
    assert(lt4a.isEmpty() && "Test 5 Failed: Not empty after extract");

    lt4b.insert(200);
    lt4b.mergeWith(lt4a);
    assert(lt4b.getMin() == 200 && "Test 5 Failed: Merging empty into non-empty");
    assert(lt4a.isEmpty() && "Test 5 Failed: Empty tree changed after merge");
    assert(lt4b.extractMin() == 200 && "Test 5 Failed: Extract after merge empty into non-empty");
    assert(lt4b.isEmpty() && "Test 5 Failed: Not empty after extract");
    cout << "Test 5 Passed." << endl;


    // Test 6: Operations on empty tree (should throw exceptions)
    LeftistTree emptyLt;
    assert(emptyLt.isEmpty() && "Test 6 Failed: Newly created tree is not empty");
    try {
        emptyLt.getMin();
        assert(false && "Test 6 Failed: getMin on empty tree did not throw");
    } catch (const runtime_error& e) {
        assert(string(e.what()) == "Heap is empty!" && "Test 6 Failed: getMin exception message incorrect");
    }
    try {
        emptyLt.extractMin();
        assert(false && "Test 6 Failed: extractMin on empty tree did not throw");
    } catch (const runtime_error& e) {
        assert(string(e.what()) == "Heap is empty! Cannot extract min." && "Test 6 Failed: extractMin exception message incorrect");
    }
    cout << "Test 6 Passed." << endl;

     // Test 7: Merging a tree with itself
    LeftistTree lt7;
    lt7.insert(50);
    lt7.insert(30);
    lt7.insert(70);
    lt7.mergeWith(lt7);

    assert(lt7.getMin() == 30 && "Test 7 Failed: Merging with self changed min");
    vector<int> extracted_self_merge;
    while(!lt7.isEmpty()) {
        extracted_self_merge.push_back(lt7.extractMin());
    }
     vector<int> expected_self_merge = {30, 50, 70};
    assert(extracted_self_merge == expected_self_merge && "Test 7 Failed: Merging with self corrupted tree");
    cout << "Test 7 Passed." << endl;


    cout << "All LeftistTree tests passed!" << endl;
}

void runLeftistTreeSample() {
    LeftistTree lt;

    cout << "Inserting elements: 10, 5, 20, 3, 15, 2" << endl;
    lt.insert(10);
    lt.insert(5);
    lt.insert(20);
    lt.insert(3);
    lt.insert(15);
    lt.insert(2);

    cout << "\nTree structure after insertions:" << endl;
    lt.printTree();

    cout << "\nMin element: " << lt.getMin() << endl;

    cout << "\nExtracting min elements:" << endl;
    while (!lt.isEmpty()) {
        cout << "Extracted: " << lt.extractMin() << endl;
        if (!lt.isEmpty()) {
             cout << "Current Tree Structure:" << endl;
             lt.printTree();
             cout << "Next Min: " << lt.getMin() << endl;
        } else {
            cout << "Tree is now empty." << endl;
        }
        cout << "----" << endl;
    }

    cout << "\nTesting merge operation:" << endl;
    LeftistTree lt1, lt2;
    lt1.insert(10);
    lt1.insert(20);
    lt1.insert(5);
    cout << "Tree 1:" << endl;
    lt1.printTree();


    lt2.insert(15);
    lt2.insert(8);
    lt2.insert(25);
    cout << "\nTree 2:" << endl;
    lt2.printTree();

    lt1.mergeWith(lt2);
    cout << "\nTree 1 after merging with Tree 2:" << endl;
    lt1.printTree();

    cout << "\nTree 2 after being merged (should be empty):" << endl;
    lt2.printTree();

    cout << "\nExtracting from merged tree:" << endl;
     while (!lt1.isEmpty()) {
        cout << "Extracted: " << lt1.extractMin() << endl;
    }

}

int main() {
    testLeftistTree();
    runLeftistTreeSample();
    return 0;
}