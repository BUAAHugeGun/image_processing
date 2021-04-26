#include "frame.h"

void channel::alloc(uint32_t h, uint32_t w) {
    data = new double *[h + 1];
    for (int i = 0; i < h; i++)
        data[i] = new double[w + 1];
    height = h, width = w;
}

void channel::set_pixel(uint32_t r, uint32_t c, double value) {
    this->data[r][c] = value;
}

double channel::get_pixel(uint32_t r, uint32_t c) {
    return this->data[r][c];
}

void frame::read_bmp(std::string &file_name) {
    io.read_file(file_name);
    decode_bmp();
}

void frame::decode_bmp() {
    //BM
    if ((char) io.next_uint(1) != 'B' || (char) io.next_uint(1) != 'M') {
        printf("this is not a BMP file!\n");
        assert(0);
    }
    //file size
    if (io.get_file_size() != io.next_uint(4)) {
        printf("file size check failed!\n");
        assert(0);
    }
    printf("file size: %u\n", io.get_file_size());
    //zero
    if (io.next_uint(4) != 0) {
        printf("zero check failed!\n");
        assert(0);
    }
    //header length
    header_length = io.next_uint(4);
    printf("header length: %u\n", header_length);
    //info header length, w, h
    info_header_length = io.next_uint(4);
    printf("info header length: %u\n", info_header_length);
    width = io.next_uint(4);
    printf("image width: %u\n", width);
    height = io.next_uint(4);
    printf("image height: %u\n", height);
    //bit plane
    if (io.next_uint(2) != 1) {
        printf("bit plane check failed!\n");
        assert(0);
    }
    //depth
    depth = io.next_uint(2);
    printf("image depth: %u\n", depth);
    //compress
    if (io.next_uint(4) != 0) {
        printf("compression check failed!\n");
        assert(0);
    }
    //image data length
    image_data_length = io.next_uint(4);
    printf("image data length: %u\n", image_data_length);
    if (image_data_length + header_length != io.get_file_size()) {
        printf("image data length check failed!\n");
        assert(0);
    }
    //x,y pixels per meter
    xppm = io.next_uint(4);
    yppm = io.next_uint(4);
    printf("pixels per meter: (%u,%u)\n", xppm, yppm);
    //color used
    color_used = io.next_uint(4);
    im_color_used = io.next_uint(4);
    printf("color used: (%u,%u)\n", color_used, im_color_used);
    //image data
    io.next_uint(header_length - 54);
    channels[CO_RED].alloc(height, width);
    channels[CO_GREEN].alloc(height, width);
    channels[CO_BLUE].alloc(height, width);

    uint32_t width_left = ((width * 3 - 1) / 4 + 1) * 4 - 3 * width;
    for (int i = (int) height - 1; i >= 0; i--) {
        for (int j = 0; j < (int) width; j++) {
            channels[CO_BLUE].set_pixel(i, j, (double) (io.next_uint(1)));
            channels[CO_GREEN].set_pixel(i, j, (double) (io.next_uint(1)));
            channels[CO_RED].set_pixel(i, j, (double) (io.next_uint(1)));
        }
        io.next_uint(width_left);
    }
    std::cout << "read bmp successfully!\n";
}

uint32_t double2uint(double x) {
    int32_t ret = round(x);
    if (ret < 0)ret = 0;
    if (ret > 255)ret = 255;
    return (uint32_t) ret;
}

void frame::encode_bmp() {
    //BM
    io.set_uint('B', 1);
    io.set_uint('M', 1);
    //file size
    io.set_uint(io.get_file_size(), 4);
    printf("file size: %u\n", io.get_file_size());
    //zero
    io.set_uint(0, 4);
    //header length
    io.set_uint(54, 4);
    printf("header length: %u\n", 54);
    //info header length, w, h
    io.set_uint(40, 4);
    printf("info header length: %u\n", 40);
    io.set_uint(width, 4);
    printf("image width: %u\n", width);
    io.set_uint(height, 4);
    printf("image height: %u\n", height);
    //bit plane
    io.set_uint(1, 2);
    //depth
    io.set_uint(depth, 2);
    printf("image depth: %u\n", depth);
    //compress
    io.set_uint(0, 4);
    //image data length
    io.set_uint(image_data_length, 4);
    printf("image data length: %u\n", image_data_length);
    //x,y pixels per meter
    io.set_uint(xppm, 4);
    io.set_uint(yppm, 4);
    printf("pixels per meter: (%u,%u)\n", xppm, yppm);
    //color used
    io.set_uint(color_used, 4);
    io.set_uint(color_used, 4);
    printf("color used: (%u,%u)\n", color_used, im_color_used);
    //image data
    uint32_t width_left = ((width * 3 - 1) / 4 + 1) * 4 - 3 * width;
    for (int i = (int) height - 1; i >= 0; i--) {
        for (int j = 0; j < (int) width; j++) {
            io.set_uint(double2uint(channels[CO_BLUE].get_pixel(i, j)), 1);
            io.set_uint(double2uint(channels[CO_GREEN].get_pixel(i, j)), 1);
            io.set_uint(double2uint(channels[CO_RED].get_pixel(i, j)), 1);
        }
        io.set_uint(0, width_left);
    }
    std::cout << "write bmp successfully!\n";
}

void frame::write_bmp(std::string &file_name) {
    io.alloc(height, width);
    encode_bmp();
    io.write_file(file_name);
}