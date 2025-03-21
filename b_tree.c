#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//order of the B-Tree
#define ORDER 3
// Structure for the node present in a B-Tree
typedef struct BTreeNode {
    int keys[ORDER - 1]; // Array of keys
    struct BTreeNode* children[ORDER]; // Array of child pointers
    int n; // Current number of keys
    bool leaf; // True if leaf node, false otherwise
} BTreeNode;
// Function to create a new B-Tree node
BTreeNode* createNode(bool isLeaf) {
    BTreeNode* newNode = (BTreeNode*)malloc(sizeof(BTreeNode));
    newNode->n = 0;
    newNode->leaf = isLeaf;
    for (int i = 0; i < ORDER; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}
// Function to split a full child node
void splitChild(BTreeNode* x, int i) {
    BTreeNode* y = x->children[i];
    BTreeNode* z = createNode(y->leaf);
    z->n = ORDER / 2 - 1;
    for (int j = 0; j < ORDER / 2 - 1; j++) {
        z->keys[j] = y->keys[j + ORDER / 2];
    }
    if (!y->leaf) {
        for (int j = 0; j < ORDER / 2; j++) {
            z->children[j] = y->children[j + ORDER / 2];
        }
    }
    y->n = ORDER / 2 - 1;
    for (int j = x->n; j >= i + 1; j--) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z;
    for (int j = x->n - 1; j >= i; j--) {
        x->keys[j + 1] = x->keys[j];
    }
    x->keys[i] = y->keys[ORDER / 2 - 1];
    x->n = x->n + 1;
}
// Function to insert a key in a non-full node
void insertNonFull(BTreeNode* x, int k) {
    int i = x->n - 1;
    if (x->leaf) {
        while (i >= 0 && k < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = k;
        x->n = x->n + 1;
    } else {
        while (i >= 0 && k < x->keys[i]) {
            i--;
        }
        i++;
        if (x->children[i]->n == ORDER - 1) {
            splitChild(x, i);
            if (k > x->keys[i]) {
                i++;
            }
        }
        insertNonFull(x->children[i], k);
    }
}
// Function to traverse the tree
void traverse(BTreeNode* x) {
    int i;
    for (i = 0; i < x->n; i++) {
        if (!x->leaf) {
            traverse(x->children[i]);
        }
        printf(" %d", x->keys[i]);
    }
    if (!x->leaf) {
        traverse(x->children[i]);
    }
}
// Function to search a key in the tree
BTreeNode* search(BTreeNode* x, int k) {
    int i = 0;
    while (i < x->n && k > x->keys[i]) {
        i++;
    }
    if (i < x->n && k == x->keys[i]) {
        return x;
    }
    if (x->leaf) {
        return NULL;
    }
    return search(x->children[i], k);
}
// Function to find the predecessor
int getPredecessor(BTreeNode* node, int idx) {
    BTreeNode* current = node->children[idx];
    while (!current->leaf) {
        current = current->children[current->n];
    }
    return current->keys[current->n - 1];
}
// Function to find the successor
int getSuccessor(BTreeNode* node, int idx) {
    BTreeNode* current = node->children[idx + 1];
    while (!current->leaf) {
        current = current->children[0];
    }
    return current->keys[0];
}
// Function to fill child node
void fill(BTreeNode* node, int idx) {
    if (idx != 0 && node->children[idx - 1]->n >= ORDER / 2) {
        borrowFromPrev(node, idx);
    } else if (idx != node->n && node->children[idx + 1]->n >= ORDER / 2) {
        borrowFromNext(node, idx);
    } else {
        if (idx != node->n) {
            merge(node, idx);
        } else {
            merge(node, idx - 1);
        }
    }
}
// Function to borrow from previous sibling
void borrowFromPrev(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx - 1];
    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }
    child->keys[0] = node->keys[idx - 1];
    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }
    node->keys[idx - 1] = sibling->keys[sibling->n - 1];
    child->n += 1;
    sibling->n -= 1;
}
// Function to borrow from next sibling
void borrowFromNext(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];
    child->keys[child->n] = node->keys[idx];
    if (!child->leaf) {
        child->children[child->n + 1] = sibling->children[0];
    }
    node->keys[idx] = sibling->keys[0];
    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }
    child->n += 1;
    sibling->n -= 1;
}
// Function to merge two nodes
void merge(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];
    child->keys[ORDER / 2 - 1] = node->keys[idx];

    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + ORDER / 2] = sibling->keys[i];
    }
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + ORDER / 2] = sibling->children[i];
        }
    }
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }
    for (int i = idx + 2; i <= node->n; ++i) {
        node->children[i - 1] = node->children[i];
    }
    child->n += sibling->n + 1;
    node->n--;
    free(sibling);
}
// Function to remove a key from a non-leaf node
void removeFromNonLeaf(BTreeNode* node, int idx) {
    int k = node->keys[idx];
    if (node->children[idx]->n >= ORDER / 2) {
        int pred = getPredecessor(node, idx);
        node->keys[idx] = pred;
        remove(node->children[idx], pred);
    } else if (node->children[idx + 1]->n >= ORDER / 2) {
        int succ = getSuccessor(node, idx);
        node->keys[idx] = succ;
        remove(node->children[idx + 1], succ);
    } else {
        merge(node, idx);
        remove(node->children[idx], k);
    }
}
// Function to remove a key from a leaf node
void removeFromLeaf(BTreeNode* node, int idx) {
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }
    node->n--;
}
// Function to remove a key from the tree
void remove(BTreeNode* node, int k) {
    int idx = 0;
    while (idx < node->n && node->keys[idx] < k) {
        ++idx;
    }
    if (idx < node->n && node->keys[idx] == k) {
        if (node->leaf) {
            removeFromLeaf(node, idx);
        } else {
            removeFromNonLeaf(node, idx);
        }
    } else {
        if (node->leaf) {
            printf("The key %d is not present in the tree\n", k);
            return;
        }
        bool flag = ((idx == node->n) ? true : false);
        if (node->children[idx]->n < ORDER / 2) {
            fill(node, idx);
        }
        if (flag && idx > node->n) {
            remove(node->children[idx - 1], k);
        } else {
            remove(node->children[idx], k);
        }
    }
}
BTreeNode* root;
void insert(int k) {
    if (root->n == ORDER - 1) {
        BTreeNode* s = createNode(false);
        s->children[0] = root;
        root = s;
        splitChild(s, 0);
        insertNonFull(s, k);
    } else {
        insertNonFull(root, k);
    }
}
void traverse() {
    if (root != NULL) {
        traverseNode(root);
    }
}
BTreeNode* search(int k) {
    return (root == NULL) ? NULL : searchNode(root, k);
}
int main() {
    root = createNode(true);
    insert(10);
    insert(20);
    insert(5);
    insert(6);
    insert(12);
    insert(30);
    insert(7);
    insert(17);
    printf("Traversal of the constructed tree is: ");
    traverse(root);
    printf("\n");
    int k = 6;
    (search(root,k) != NULL) ? printf("%d is found\n", k) : printf("%d is not found\n", k);
    k = 15;
    (search(root,k) != NULL) ? printf("%d is found\n", k) : printf("%d is not found\n", k);
    remove(root, 6);
    printf("Traversal of the tree after removing 6: ");
    traverse(root);
    printf("\n");
    remove(root, 13);
    printf("Traversal of the tree after removing 13: ");
    traverse(root);
    printf("\n");
    return 0;
}
