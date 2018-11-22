#include <cstdio>
#include <algorithm>

using key_t = unsigned long long;
using val_t = unsigned long long;

FILE * ifp, * ofp;

struct Node {
    Node(key_t key, val_t val) : key(key), val(val) {}
    inline int get_height_diff();
    inline void recalc_my_height();
    key_t key;
    val_t val;
    Node * left  = nullptr;
    Node * right = nullptr;
    int height = 0;
};

Node * root;

// Height of left
int Node::get_height_diff() {
    int left_height = left ? left->height : -1;
    int right_height = right ? right->height : -1;
    return left_height - right_height;
}

// Recalculate my height
void Node::recalc_my_height() {
    int left_height = left ? left->height : -1;
    int right_height = right ? right->height : -1;
    height = std::max(left_height, right_height) + 1;
}

Node * rotate_R(Node * root) {
    Node * child = root->left;
    root->left = child->right;
    child->right = root;
    root->recalc_my_height();
    child->recalc_my_height();
    return child;
}

Node * rotate_L(Node * root) {
    Node * child = root->right;
    root->right = child->left;
    child->left = root;
    root->recalc_my_height();
    child->recalc_my_height();
    return child;
}

Node * rotate_LR(Node * root) {
    root->left = rotate_L(root->left);
    return rotate_R(root);
}

Node * rotate_RL(Node * root) {
    root->right = rotate_R(root->right);
    return rotate_L(root);
}

// Insert 'node' into tree whose root is 'root'.
// Returns new root.
Node * insert_into_tree(Node * root, Node * node) {
    // Reached end of the tree. Make new node.
    if (!root) {
        fprintf(ofp, "Inserted (%llu,%llu)\n", node->key, node->val);
        return node;
    }

    // Recursive insert phase
    if (root->key == node->key) {
        root->val = node->val;
        fprintf(ofp, "Found (%llu,%llu) update v=%llu\n", root->key, root->val, node->val);
        return root;
    }
    if (root->key > node->key) {
        root->left = insert_into_tree(root->left, node);
    } else {
        root->right = insert_into_tree(root->right, node);
    }

    root->recalc_my_height();

    // Rebalance phase
    if        (root->get_height_diff() >= 2) {    // left tree is much deeper
        return root->left->get_height_diff() >= 1 ?
            rotate_R(root) : rotate_LR(root);
    } else if (root->get_height_diff() <= -2) {   // Right tree is much deeper
        return root->right->get_height_diff() <= -1 ?
            rotate_L(root) : rotate_RL(root);
    }
    
    return root;
}

void insert(key_t key, val_t val) {
    Node * node = new Node(key, val);
    if (!root) {
        root = node;
        fprintf(ofp, "Inserted (%llu,%llu)\n", node->key, node->val);
    }
    else root = insert_into_tree(root, node);
}

void find(key_t key) {
    int d = 0;
    Node * node = root;
    while (node) {
        if (node->key == key) {
            fprintf(ofp, "Found (%llu, %llu) on d=%d with h=%d\n", node->key, node->val, d, node->height);
            return;
        }
        node = (node->key > key ? node->left : node->right);
        d++;
    }
    fprintf(ofp, "Not Found\n");
}

void print_preorder(Node * root) {
    fprintf(ofp, "(%llu,%llu) ", root->key, root->val);
    if (root->left) print_preorder(root->left);
    if (root->right) print_preorder(root->right);
}

void printTree() {
    print_preorder(root);
    fprintf(ofp, "\n");
}

int main(int argc, char * argv[]) {
    ifp = fopen((argc == 1 ? "input.txt" : argv[1]), "r");
    ofp = fopen("output.txt", "w");

    char cmd;
    key_t key;
    val_t val;

    while (fscanf(ifp, " %c", &cmd), cmd != 'Q') {
        switch (cmd) {
        case 'I': 
            fscanf(ifp, " %llu %llu", &key, &val);
            insert(key, val);
            break;
        
        case 'F': 
            fscanf(ifp, " %llu", &key);
            find(key);
            break;
        
        case 'P':
            printTree();
        }
    }
    return 0;
}