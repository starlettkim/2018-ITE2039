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
    FIO(const char * in_file, const char * out_file) : r_idx(0), w_idx(0) {
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

        // TODO: Allocate out_file into memory if it is successfully opened.
        if ((ofd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) >= 0) {
            // fstat(ifd, &status);
        }
    }

    ~FIO() {
        munmap(r_mem, r_sz);
        free(w_buf);
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
        w_buf = new unsigned char[128];
        int idx = 0;
        
        while (val) {
            w_buf[128 - ++idx] = val % 10 + '0';
            val /= 10;
        }
        write(STDOUT_FILENO, w_buf + 128 - idx, sizeof(unsigned char) * idx);
        write(ofd, w_buf + 128 - idx, sizeof(unsigned char) * idx);
    }

    void write_char(char c) {
        write(STDOUT_FILENO, &c, sizeof(char));
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
    T * arr;
    int cap, sz;

    MinHeap(int cap) : cap(cap), sz(0) {
        arr = new T[cap + 1];
        
    }

    ~MinHeap() {
        free(arr);
    }

    T extract_min(void) {
        
    }
    
    void insert(T value) {
        if (sz == cap) return;
        int i;
        for (i = ++sz; i > 1 && arr[i / 2] < value; i /= 2) {
            arr[i] = arr[i / 2];
        }
        arr[i] = value;
    }
};

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
    value_t ** arr = new value_t* [N];

    for (int i = 0; i < N; i++) {
        arr[i] = new value_t[M];
        free(in);
        sprintf(ifn, "input%d.txt", i + 1);
        in = new FIO(ifn, NULL);

        for (int j = 0; j < M; j++) {
            arr[i][j] = in->read_int<value_t>();
        }
    }
    free(in);
    
    return 0;

}