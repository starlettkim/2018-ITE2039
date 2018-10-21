#include <iostream>

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include <random>
#include <limits>

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

struct Hash {
    Hash(const char * filename) {
        FIO fio(filename, nullptr);
        fio.read_int(sz);
        hash_table = new H1[sz];
        default_random_engine gen;
        uniform_int_distribution<_key_t> distr(numeric_limits<_key_t>::min(), numeric_limits<_key_t>::max());

        for (int i = 0; i < sz; i++) {
            _key_t key;
            fio.read_int(key);
            int idx = find_bucket(key);
            hash_table[idx].m++;
            hash_table[i].a = distr(gen);
        }

        for (int i = 0; i < sz; i++) {
            hash_table[i].m *= hash_table[i].m;
            hash_table[i].hash_table = new H2[hash_table[i].m];
        }
    }

    ~Hash() { 
        for (int i = 0; i < sz; i++) {
            delete[] hash_table[i].hash_table;
        }
        delete[] hash_table;
    }

    void _insert(_key_t key, _val_t val) {
        int idx = find_bucket(key);
        H1 & bucket = hash_table[idx];
        idx = 0;
        int m = bucket.m;
        int a = bucket.a;
        _key_t k = key;
        while (k) {
            idx += (a % 10) * (k % 10);
            a /= 10;
            k /= 10;
            idx %= m;
        }
        while (bucket.hash_table[idx].is_up) {
            if (bucket.hash_table[idx].key == key) {
                bucket.hash_table[idx].val = val;
                throw "Duplicated\n";
            }
            idx = (idx + 1) % m;
        }
        bucket.hash_table[idx].key = key;
        bucket.hash_table[idx].val = val;
    }
    
    void _delete(_key_t key) {
        int idx = find_bucket(key);
        H1 & bucket = hash_table[idx];
        idx = 0;
        int m = bucket.m;
        int a = bucket.a;
        _key_t k = key;
        while (k) {
            idx += (a % 10) * (k % 10);
            a /= 10;
            k /= 10;
            idx %= m;
        }
        for (int i = 0; i < bucket.m; i++) {
            int j = idx + i;
            if (bucket.hash_table[j].is_up && bucket.hash_table[j].key == key) {
                bucket.hash_table[j].is_up = false;
                return;
            }
            if (!bucket.hash_table[j].is_up) {
                throw "Not Found\n";
            }
        }
    }

    _val_t _find(_key_t key) {
        
    }

    void _print() {
        
    }

private:
    struct H2 {
        bool is_up;
        _key_t key;
        _val_t val;
    };
    struct H1 {
        _key_t a;
        int m;
        H2 * hash_table;
    };
    int sz;
    H1 * hash_table;

    inline int find_bucket(_key_t key) {
        return key % sz;
    }
    int find_pos(_key_t key) {
        int idx = find_bucket(key);
        int ret = 0;
        int m = hash_table[idx].m;
        int a = hash_table[idx].a;
        _key_t k = key;
        while (key) {
            ret += (a % 10) * (k % 10);
            a /= 10;
            k /= 10;
            ret %= m;
        }
        while (hash_table[idx].hash_table[ret].is_up) {
            ret = (ret + 1) % m;
            num_collision++;
        }
        return ret;
    }

    int num_collision = 0;
};

int main(int argc, const char * argv[]) {
    if (argc < 2) argv[1] = "input.txt";
    if (argc < 3) argv[2] = "input-keys.txt";
    Hash hash(argv[2]);
    FIO fio(argv[1], "output.txt");

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

        } else if (ctrl == 'P') {
            hash._print();
        } else break;
    }

    return 0;
}