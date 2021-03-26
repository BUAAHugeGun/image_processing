#include "frame.h"

void frame::read_bmp(std::string &file_name) {
    io.read_file(file_name);
    decode_bmp();
}

void frame::decode_bmp() {
    if ((char) io.next_uint(1) != 'B' || (char) io.next_uint(1) != 'M') {
        printf("this is not a BMP file!\n");
        assert(0);
    }
    int x;
    if (io.get_file_size() != (x = io.next_uint(4))) {
        std::cerr << io.get_file_size() << " " << x << "\n";
        printf("file size check failed!\n");
        assert(0);
    }
    printf("file size: %u\n", io.get_file_size());

    if (io.next_uint(4) != 0) {
        printf("Zero check failed!\n");
        assert(0);
    }

    header_length = io.next_uint(4);

    // bitmap info header
    info_header_length = io.next_uint(4);
    width = io.next_uint(4);
    printf("image width: %u\n", width);
    height = io.next_uint(4);
    printf("image height: %u\n", height);

    if (io.next_uint(2) != 1) {
        printf("bit plane check failed!\n");
        assert(0);
    }

    depth = io.next_uint(2);
    printf("image depth: %u\n", depth);

    if (io.next_uint(4) != 0) {
        printf("compression check failed!\n");
        assert(0);
    }
}