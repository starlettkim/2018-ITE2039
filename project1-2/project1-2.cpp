#include <cstdio>
#include <cstdint>
#include <iostream>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

typedef uint64_t value_t;

// Struct for fast I/O.
struct FIO {
private:
    int ifd, ofd;

    size_t r_sz, r_idx;
    // size_t w_sz, w_idx;
    unsigned char * r_mem;
    unsigned char * w_buf;

public:
    FIO(const char * in_file, const char * out_file) : r_idx(0) {
        struct stat status;

        // Allocate in_file into memory if it is successfully opened.
        if ((ifd = open(in_file, O_RDONLY)) >= 0) {
            fstat(ifd, &status);
            r_sz = status.st_size;
            r_mem = static_cast<unsigned char *>(
                mmap(
                    NULL,
                    r_sz,
                    PROT_READ,
                    MAP_FILE | MAP_PRIVATE,
                    ifd,
                    0
                )
            );
        }

        w_buf = new unsigned char[128];

        // TODO: Allocate out_file into memory if it is successfully opened.
        if ((ofd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) >= 0) {
            // fstat(ifd, &status);
        }
    }

    ~FIO() {
        munmap(r_mem, r_sz);
        delete[] w_buf;
        close(ifd);
        close(ofd);
    }

    template <typename T>
    inline T read_int() {
        T ret = 0;
        int neg = 0;
        consume_whitespace();
        if (r_mem[r_idx] == '-') {
            neg = 1;
            r_idx++;
        }
        while (r_idx < r_sz && !is_whitespace()) {
            ret = ret * 10 + r_mem[r_idx++] - '0';
        }
        return neg ? -ret : ret;
    }

    template <typename T>
    void write_int(T val) {
        if (val < 0) {
            write(ofd, "-", 1);
            val *= -1;
        }
        int idx = 0;
        
        while (val) {
            w_buf[128 - ++idx] = val % 10 + '0';
            val /= 10;
        }
        write(ofd, w_buf + 128 - idx, sizeof(unsigned char) * idx);
    }

    void write_char(char c) {
        write(ofd, &c, sizeof(char));
    }

private:
    inline bool is_whitespace() {
        return
            r_mem[r_idx] == '\n' || r_mem[r_idx] == '\r' || r_mem[r_idx] == '\t' ||
            r_mem[r_idx] == '\f' || r_mem[r_idx] == '\v' || r_mem[r_idx] == ' ';
    }

    inline void consume_whitespace() {
        while (r_idx < r_sz && is_whitespace()) r_idx++;
    }
};

template <typename T>
struct MinHeap {
private:
    T * arr;
    int cap, sz;

public:
    MinHeap(int cap) : cap(cap), sz(0) {
        arr = new T[cap + 1];
    }

    ~MinHeap() {
        delete[] arr;
    }

    T pop(void) {
        T min_val = arr[1];
        T last_val = arr[sz--];
        int i, child;
        for (i = 1; i * 2 <= sz; i = child) {
            child = i * 2;
            if (child < sz && arr[child + 1] < arr[child]) {
                child++;
            }
            if (arr[child] < last_val) {
                arr[i] = arr[child];
            } else break;
        }
        arr[i] = last_val;
        return min_val;
    }
    
    void insert(T val) {
        if (sz == cap) return;
        int i;
        for (i = ++sz; i > 1 && val < arr[i / 2]; i /= 2) {
            arr[i] = arr[i / 2];
        }
        arr[i] = val;
    }

    bool empty() {
        return !sz;
    }
};

struct Pair {
    value_t val;
    int file_no;
};

bool operator < (const Pair & lhs, const Pair & rhs) {
    return lhs.val < rhs.val;
}

int main() {
    char * ifn = new char[20];
    char * ofn = new char[20];
    sprintf(ifn, "input.txt");
    sprintf(ofn, "output.txt");
    FIO *  in  = new FIO(ifn, NULL);
    FIO *  out = new FIO(NULL, ofn);

    int N = in->read_int<int>();
    int M = in->read_int<int>();
    int K = in->read_int<int>();

    delete in;
    
    MinHeap<Pair> heap(N);
    FIO ** files = new FIO *[N];    // FIO array for input files.
    int * n_read = new int[N];      // number of values read from file.
    for (int i = 0; i < N; i++) {
        sprintf(ifn, "input%d.txt", i + 1);
        files[i] = new FIO(ifn, NULL);
        value_t first_value = files[i]->read_int<value_t>();
        heap.insert({ first_value, i });
        n_read[i]++;
    }

    while (!heap.empty()) {
        for (int i = 1; i <= K; i++) {
            if (!heap.empty()) {
                auto poped = heap.pop();
                if (n_read[poped.file_no]++ < M) {      // Read next value.
                    value_t nxt_value = files[poped.file_no]->read_int<value_t>();
                    heap.insert({ nxt_value, poped.file_no });
                } else delete files[poped.file_no];     // Nothing left to read.

                if (i == K) {   // Write every K-th value to output file.
                    out->write_int(poped.val);
                    out->write_char(' ');
                } // else do nothing
            }
        }
    }

    delete[] files;
    delete[] n_read;

    return 0;
}