#include <stdio.h>

using key_t = unsigned long long;
using val_t = unsigned long long;

FILE * g_ifp, * g_ofp;

struct Node {
    Node(key_t key, val_t val) : key(key), val(val) {}
    void rebalance();
    Node * rotate_R();
    Node * rotate_L();
    key_t key;
    val_t val;
    Node * left   = nullptr;
    Node * right  = nullptr;
    Node * parent = nullptr;
    bool is_red = 1;
};

Node * g_root;

Node * Node::rotate_R() {
    Node * child = left;
    left = child->right;
    if (left) left->parent = this;
    child->right = this;
    this->parent = child;
    return child;
}

Node * Node::rotate_L() {
    Node * child = right;
    right = child->left;
    if (right) right->parent = this;
    child->left = this;
    this->parent = child;
    return child;
}

void Node::rebalance() {
    Node * g, * u;
    g = parent ? parent->parent : nullptr;
    if (!g) u = nullptr;
    else u = g->left == parent ? g->right : g->left;

    if (!parent) {  // root.
        is_red = false;
        return;
    }
    if (!parent->is_red) return;    // Do nothing.

    if (u && u->is_red) {   // u = red, g = black.
        g->is_red = true;
        u->is_red = false;
        parent->is_red = false;

    } else {                // u = black, g = black.
        Node * new_n = parent;
        Node * new_p = g;
        Node * new_g = g->parent;

        // Transform to Case 3.
        if (this == parent->left && parent == g->right) {
            g->right = parent->rotate_R();
            new_n = this;
        } else if (this == parent->right && parent == g->left) {
            g->left = parent->rotate_L();
            new_n = this;
        }

        // Case 3.
        if (!new_g) {
            new_n == new_p->left ? new_p->rotate_R() : new_p->rotate_L();
        } else if (new_p == new_g->left) {
            new_g->left = (new_n == new_p->left ? new_p->rotate_R() : new_p->rotate_L());
        } else {
            new_g->right = (new_n == new_p->left ? new_p->rotate_R() : new_p->rotate_L());
        }
        new_n->is_red = false;
        new_p->is_red = true;
    }
}

Node * insert_into_tree(Node * root, Node * node) {
    if (!root) {
        fprintf(g_ofp, "Inserted (%llu,%llu)\n", node->key, node->val);
        return node;
    }

    // Recursive insert
    if (root->key == node->key) {
        root->val = node->val;
        fprintf(g_ofp, "Found (%llu,%llu) update v=%llu\n", root->key, root->val, node->val);
        return root;
    }
    if (root->key > node->key) {
        root->left = insert_into_tree(root->left, node);
        root->left->parent = root;
        if (root->left->is_red) root->left->rebalance();
    } else {
        root->right = insert_into_tree(root->right, node);
        root->right->parent = root;
        if (root->right->is_red) root->right->rebalance();
    }

    return root;
}

void insert(key_t key, val_t val) {
    Node * node = new Node(key, val);
    if ((g_root = insert_into_tree(g_root, node))->is_red)
        g_root->rebalance();
}

void find(key_t key) {
    int d = 0;
    Node * node = g_root;
    while (node) {
        if (node->key == key) {
            fprintf(g_ofp, "Found (%llu,%llu) on d=%d with c=%s\n",
                node->key, node->val, d, node->is_red ? "red" : "black");
            return;
        }
        node = (node->key > key ? node->left : node->right);
        d++;
    }
    fprintf(g_ofp, "Not Found\n");
}

void print_preorder(Node * root) {
    fprintf(g_ofp, "(%llu,%llu) ", root->key, root->val);
    if (root->left) print_preorder(root->left);
    if (root->right) print_preorder(root->right);
}

void printTree() {
    print_preorder(g_root);
    fprintf(g_ofp, "\n");
}

int main(int argc, char * argv[]) {
    g_ifp = fopen((argc == 1 ? "input_small.txt" : argv[1]), "r");
    g_ofp = fopen("output.txt", "w");

    char cmd;
    while (fscanf(g_ifp, "%c", &cmd), cmd != 'Q') {
        key_t key;
        val_t val;
        switch (cmd) {
        case 'I': 
            fscanf(g_ifp, " %llu %llu", &key, &val);
            insert(key, val);
            break;
        
        case 'F': 
            fscanf(g_ifp, " %llu", &key);
            find(key);
            break;
        
        case 'P':
            printTree();
        }
    }
    return 0;
}