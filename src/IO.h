#ifndef SRC_IO_H
#define SRC_IO_H

#include <bits/stdc++.h>

class IO {
    std::string file_name;
    uint32_t file_size;
    unsigned char *buffer_tmp;
    unsigned char *buffer = nullptr;
public:
    ~IO() {
        delete buffer;
    }

    void read_file(std::string &file_name);

    uint32_t get_file_size();

    uint32_t next_uint(uint32_t length);
};


#endif //SRC_IO_H
