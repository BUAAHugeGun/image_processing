#include "IO.h"

IO::IO(const IO &rhs) {
    this->file_name = rhs.file_name;
    this->file_size = rhs.file_size;
    if (this->buffer != nullptr) {
        delete this->buffer;
    }
    this->alloc(rhs.file_size);
    std::memcpy(this->buffer, rhs.buffer, rhs.file_size + 1);
}

IO &IO::operator=(IO rhs) {
    IO::swap(*this, rhs);
    return *this;
}

void IO::swap(IO &a, IO &b) {
    std::swap(a.file_name, b.file_name);
    std::swap(a.file_size, b.file_size);
    std::swap(a.buffer, b.buffer);
    std::swap(a.buffer_tmp, b.buffer_tmp);
}

void IO::alloc(uint32_t height, uint32_t width) {
    uint32_t head_length = 54;
    uint32_t data_length = height * ((width * 3 - 1) / 4 + 1) * 4;
    alloc(data_length + head_length);
}

void IO::alloc(uint32_t size) {
    delete buffer;
    set_file_size(size);
    buffer = new unsigned char[size + 10];
    buffer_tmp = buffer;
}

void IO::read_file(std::string &file_name_) {
    file_name = file_name_;
    std::FILE *fp = fopen(file_name.data(), "rb");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    rewind(fp);
    buffer = new unsigned char[this->file_size + 10];
    std::fread(buffer, 1, this->file_size, fp);
    fclose(fp);

    buffer_tmp = buffer;
}

void IO::write_file(std::string &file_name_) {
    file_name = file_name_;
    std::FILE *fp = fopen(file_name_.data(), "wb");

    std::fwrite(buffer, 1, this->file_size, fp);
    fclose(fp);
}

uint32_t IO::get_file_size() const {
    return this->file_size;
}

void IO::set_file_size(uint32_t size) {
    this->file_size = size;
}

uint32_t IO::next_uint(uint32_t length) {
    if (buffer + file_size < buffer_tmp + length - 1) {
        printf("Buffer read overflow!\n");
        assert(0);
    }
    uint32_t ret = 0;
    for (int i = 0; i < length; i++) {
        ret |= ((uint32_t) (*(buffer_tmp++))) << (i << 3);
    }
    return ret;
}

void IO::set_uint(uint32_t x, uint32_t length) {
    if (buffer + file_size < buffer_tmp + length - 1) {
        printf("Buffer write overflow!\n");
        assert(0);
    }
    for (int i = 0; i < length; i++) {
        *(buffer_tmp++) = (unsigned char) (x & ((1u << 8) - 1));
        x >>= 8;
    }
}