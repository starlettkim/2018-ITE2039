#include <cstdio>
#include <cstdint>
#include <iostream>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define INFILE  "input.txt"
#define OUTFILE "output.txt"

const int MAX_N = 1e5;
typedef uint64_t value_t;

using namespace std;

// Struct for fast I/O.
struct FIO {
    size_t sz, idx;
    int ifd, ofd;
    unsigned char * buf;

    FIO(const char * in_file, const char * out_file) : idx(0) {
        ifd = open(in_file, O_RDONLY);
        ofd = open(out_file, O_CREAT | O_WRONLY);

        struct stat status;
        fstat(ifd, &status);
        sz = status.st_size;
        buf = static_cast<unsigned char *>(
            mmap(
                NULL,
                sz,
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

    inline bool is_whitespace() {
        return
            buf[idx] == '\n' || buf[idx] == '\r' || buf[idx] == '\t' ||
            buf[idx] == '\f' || buf[idx] == '\v' || buf[idx] == ' ';
    }

    inline void consume_whitespace() {
        while (idx < sz && is_whitespace()) idx++;
    }

    template <typename T>
    inline T read_int() {
        T ret = 0;
        int neg = 0;
        consume_whitespace();
        if (buf[idx] == '-') {
            neg = 1;
            idx++;
        }
        while (idx < sz && !is_whitespace()) {
            ret = ret * 10 + buf[idx++] - '0';
        }
        return neg ? -ret : ret;
    }
};

inline int pick_pivot(value_t * target, int sz) {
    return 0;
}

template <typename T>
inline void swap(T * lhs, T * rhs) {
    T tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

void quick_sort(value_t * target, int sz) {
    if (!sz) return;
    swap(target, target + pick_pivot(target, sz));
    int i = 1;
    for (int j = 1; j < sz; j++) {
        if (target[j] <= target[0]) {
            swap(target + i, target + j);
            i++;
        }
    }
    swap(target, target + i - 1);
    quick_sort(target, i - 1);
    quick_sort(target + i, sz - i);
}

int main(int argc, char * argv[]) {
    int N;
    value_t * v_val = new value_t[MAX_N];
    FIO fio(INFILE, OUTFILE);

    N = fio.read_int<int>();
    for (int i = 0; i < N; i++)
        v_val[i] = fio.read_int<value_t>();

    quick_sort(v_val, N);
    
    cout << N << endl;
    for (int i = 0; i < N; i++)
        cout << v_val[i] << " ";
    cout << endl;

    free(v_val);
    return 0;
}