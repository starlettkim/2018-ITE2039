#include <iostream>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

using _key_t = uint64_t;
using _val_t = uint64_t;

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
    inline void read_int(T & val) {
        val = 0;
        bool neg = 0;
        consume_whitespace();
        if (r_mem[r_idx] == '-') {
            neg = 1;
            r_idx++;
        }
        while (r_idx < r_sz && !is_whitespace()) {
            val = val * 10 + r_mem[r_idx++] - '0';
        }
        if (neg) val *= -1;
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
        int idx = 127;
        while (val) {
            w_buf[idx--] = val % 10 + '0';
            val /= 10;
        }
        if (neg) w_buf[idx--] = '-';
        write(w_buf + idx + 1, 127 - idx);
    }

    void write(const char * str, int len) {
        ::write(ofd, str, len);
        fsync(ofd);
    }

    void write(char c) {
        ::write(ofd, &c, sizeof(char));
        fsync(ofd);
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

struct Record {
    bool is_up;
    _key_t key;
    _val_t val;
};

struct Hash {
    Hash(int sz) : sz(sz) {
        records = new Record[sz];
    }

    ~Hash() { delete[] records; }

    void _insert(_key_t key, _val_t val) {
        for (int i = 0; i < sz; i++) {
            int idx = run_hash(key, i);
            if (records[idx].is_up && records[idx].key == key) {
                records[idx].val = val;
                throw "Duplicated\n";
            }
            if (!records[idx].is_up) {
                records[idx].is_up = true;
                records[idx].key = key;
                records[idx].val = val;
                return;
            }
        }
        throw "Full\n";
    }
    
    void _delete(_key_t key) {
        for (int i = 0; i < sz; i++) {
            int idx = run_hash(key, i);
            if (records[idx].is_up && records[idx].key == key) {
                records[idx].is_up = false;
                return;
            }
            if (!records[idx].is_up) {
                throw "Not Found\n";
            }
        }
        throw "Not Found\n";
    }

    _val_t _find(_key_t key) {
        for (int i = 0; i < sz; i++) {
            int idx = run_hash(key, i);
            if (records[idx].is_up && records[idx].key == key) {
                return records[idx].val;
            }
            if (!records[idx].is_up) {
                throw "Not Found\n";
            }
        }
        throw "Not Found\n";
    }

    void _print() {

    }

private:
    int sz;
    Record * records;
    int run_hash(_key_t key, int trial = 0) {
        return (key + trial) % sz;
    }

};

int main(int argc, const char * argv[]) {
    if (argc < 2) argv[1] = "input.txt";
    FIO fio(argv[1], "output.txt");
    int N;
    fio.read_int(N);
    Hash hash(3 * N);
    _key_t key;
    _val_t val;
    while (true) {
        char ctrl = fio.read_char();
        if        (ctrl == 'I') {
            try {
                fio.read_int(key);
                fio.read_int(val);
                hash._insert(key, val);
                fio.write("Inserted\n", 9);
            } catch (const char * err) {
                fio.write(err, strlen(err));
            }

        } else if (ctrl == 'D') {
            try {
                fio.read_int(key);
                hash._delete(key);
                fio.write("Deleted\n", 8);
            } catch (const char * err) {
                fio.write(err, strlen(err));
            }

        } else if (ctrl == 'F') {
            try {
                fio.read_int(key);
                val = hash._find(key);
                fio.write_int(val);
                fio.write('\n');
            } catch (const char * err) {
                fio.write(err, strlen(err));
            }

        } else break;
    }

    return 0;
}