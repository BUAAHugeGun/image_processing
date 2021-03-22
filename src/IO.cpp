#include "IO.h"

void IO::read_file(std::string &file_name) {
    this->file_name = file_name;
    std::FILE *fp = fopen(file_name.data(), "rt");

    fseek(fp, 0, SEEK_END);
    this->file_size = ftell(fp);

    rewind(fp);
    buffer = new char[this->file_size + 10];
    std::fread(buffer, 1, this->file_size);
    fclose(fp);

    buffer_tmp = buffer;
}

const char *IO::get_buffer() {
    return this->buffer;
}

uint32_t IO::get_file_size() {
    return this->file_size;
}

char IO::next_char() {
    if (buffer == buffer_tmp) {
        printf("Buffer overflow!\n");
        exit(-1);
    }
    return *(buffer_tmp++);
}

uint32_t IO::next_uint32() {
    if (buffer < buffer_tmp + 3) {
        printf("Buffer overflow!\n");
        exit(-1);
    }
    uint32_t ret = 0;
    for (int i = 0; i < 4; i++) {
        ret |= ((uint32_t) (*(buffer_tmp++))) << (i << 3);
    }
    return ret;
}