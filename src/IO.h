#ifndef HW_IO_H
#define HW_IO_H

#include <bits/stdc++.h>

class IO {
    std::string file_name;
    uint32_t file_size;
    char *buffer_tmp;
    char *buffer = nullptr;
public:
    ~IO() {
        delete buffer;
    }

    void read_file(std::string &file_name);

    const char *get_buffer();

    uint32_t get_file_size();

    char next_char();

    uint32_t next_uint32();
};


#endif //HW_IO_H
