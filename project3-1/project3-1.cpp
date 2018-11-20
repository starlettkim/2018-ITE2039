#include <stdio.h>

using key_t = unsigned long long;
using val_t = unsigned long long;

struct Node {
    key_t key;
    val_t val;
};

void insert(key_t key, val_t val) {

}

void find(key_t key) {

}

void printTree() {

}

int main(int argc, char * argv[]) {
    FILE * ifp = fopen((argc == 1 ? "input.txt" : argv[1]), "r");
    FILE * ofp = fopen("output.txt", "w");

    char cmd;
    while (scanf("%c", &cmd), cmd != 'Q') {
        key_t key;
        val_t val;
        switch (cmd) {
        case 'I': 
            fscanf(ifp, " %llu %llu", &key, &val);
            insert(key, val);
            break;
        
        case 'F': 

        }
    }
    return 0;
}