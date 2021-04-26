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

    void alloc(uint32_t height, uint32_t width);

    void alloc(uint32_t size);

    void read_file(std::string &file_name_);

    void write_file(std::string &file_name_);

    uint32_t get_file_size() const;

    void set_file_size(uint32_t size);

    uint32_t next_uint(uint32_t length);

    void set_uint(uint32_t x, uint32_t length);


};


#endif //SRC_IO_H
