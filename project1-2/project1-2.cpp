#include <cstdio>
#include <cstdint>
#include <iostream>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

typedef uint64_t value_t;

using namespace std;

// Struct for fast I/O.
struct FIO {
private:
    int ifd, ofd;

    size_t r_sz, r_idx;
    unsigned char * r_buf;
    unsigned char * w_buf;

public:
    FIO(const char * in_file, const char * out_file) : r_idx(0) {
        ifd = open(in_file, O_RDONLY);
        ofd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        struct stat status;
        fstat(ifd, &status);
        r_sz = status.st_size;
        r_buf = static_cast<unsigned char *>(
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

    ~FIO() {
        close(ifd);
        close(ofd);
    }

    template <typename T>
    inline T read_int() {
        T ret = 0;
        int neg = 0;
        consume_whitespace();
        if (r_buf[r_idx] == '-') {
            neg = 1;
            r_idx++;
        }
        while (r_idx < r_sz && !is_whitespace()) {
            ret = ret * 10 + r_buf[r_idx++] - '0';
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
            r_buf[r_idx] == '\n' || r_buf[r_idx] == '\r' || r_buf[r_idx] == '\t' ||
            r_buf[r_idx] == '\f' || r_buf[r_idx] == '\v' || r_buf[r_idx] == ' ';
    }

    inline void consume_whitespace() {
        while (r_idx < r_sz && is_whitespace()) r_idx++;
    }
};

int main() {
    char * ifn = "input.txt";
    char * ofn = "output.txt";
    FIO *  in  = new FIO(ifn, NULL);
    FIO *  out = new FIO(NULL, ofn);

    int N = in->read_int<int>();
    int M = in->read_int<int>();
    int K = in->read_int<int>();
    int ** arr = new int* [N];

    for (int i = 0; i < N; i++) {
        arr[i] = new int[M];
        free(in);
        sprintf(ifn, "input%d.txt", i + 1);
        in = new FIO(ifn, NULL);
        
        int j = 0;
        arr[i][j++] = in->read_int<value_t>();
    }
    
    return 0;

}