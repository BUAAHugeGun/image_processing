#include "IO.h"

void IO::read_file(std::string &file_name) {
    this->file_name = file_name;
    std::FILE *fp = fopen(file_name.data(), "rt");

    fseek(fp, 0, SEEK_END);
    this->file_size = ftell(fp);

    rewind(fp);
    buffer = new unsigned char[this->file_size + 10];
    std::fread(buffer, 1, this->file_size, fp);
    fclose(fp);

    buffer_tmp = buffer;
}

uint32_t IO::get_file_size() {
    return this->file_size;
}

uint32_t IO::next_uint(uint32_t length) {
    if (buffer + file_size < buffer_tmp + length - 1) {
        printf("Buffer overflow!\n");
        assert(0);
    }
    uint32_t ret = 0;
    for (int i = 0; i < length; i++) {
        ret |= ((uint32_t) (*(buffer_tmp++))) << (i << 3);
    }
    return ret;
}