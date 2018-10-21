#include <iostream>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

using key_t = uint64_t;
using val_t = uint64_t;

// Struct for fast I/O.
struct FIO {
private:
    int ifd, ofd;

    size_t r_sz, r_idx;
    // size_t w_sz, w_idx;
    char * r_mem;
    char * w_buf;

public:
    FIO(const char * in_file, const char * out_file) : r_idx(0) {
        struct stat status;

        // Allocate in_file into memory if it is successfully opened.
        if ((ifd = open(in_file, O_RDONLY)) >= 0) {
            fstat(ifd, &status);
            r_sz = status.st_size;
            r_mem = static_cast<char *>(
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

        w_buf = new char[128];

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
        bool neg = 0;
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

    inline char read_char() {
        consume_whitespace();
        return r_mem[r_idx++];
    }

    template <typename T>
    void write_int(T val) {
        bool neg = 0;
        if (val < 0) {
            neg = 1;
            val *= -1;
        }
        int idx = 126;
        w_buf[127] = 0;
        while (val) {
            w_buf[idx--] = val % 10 + '0';
            val /= 10;
        }
        if (neg) w_buf[idx--] = '-';
        write(w_buf + idx + 1);
    }

    void write(const char * str) {
        ::write(ofd, str, strlen(str));
    }

    void write(char c) {
        ::write(ofd, &c, sizeof(char));
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

int main(int argc, char * argv[]) {
    FIO fio(argv[1], "output.txt");
    int N = fio.read_int<int>();
    while (N--) {
        char ctrl = fio.read_char();
        if        (ctrl == 'I') {

        } else if (ctrl == 'D') {

        } else if (ctrl == 'F') {

        }
    }

    return 0;
}