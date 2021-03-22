#include "frame.h"

void frame::read_bmp(std::string &file_name) {
    io.read_file(file_name);
    decode_bmp(io.get_buffer());
}

void frame::decode_bmp() {
    if (io.next_char() != 'B' || io.next_char() != 'M') {
        printf("This is not a BMP file!\n");
        exit(-1);
    }

    if (io.get_file_size() != io.next_uint32()) {
        printf("File size check failed!\n");
        exit(-1);
    }
    

}