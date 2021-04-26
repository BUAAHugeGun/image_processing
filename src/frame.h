#ifndef SRC_FRAME_H
#define SRC_FRAME_H

#include <bits/stdc++.h>
#include "IO.h"

#define CO_RED 0
#define CO_GREEN 1
#define CO_BLUE 2
#define CO_GREY 3
#define CO_BINARY 4

class channel {
    double **data = nullptr;
    uint32_t width;
    uint32_t height;
public:
    void alloc(uint32_t h, uint32_t w);

    void set_pixel(uint32_t r, uint32_t c, double value);

    double get_pixel(uint32_t r, uint32_t c);

    ~channel() {
        if (data != nullptr)
            for (int i = 0; i < height; i++)
                delete data[i];
    }
};

class frame {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    uint32_t header_length;
    uint32_t info_header_length;
    uint32_t image_data_length;
    uint32_t xppm;
    uint32_t yppm;
    uint32_t color_used;
    uint32_t im_color_used;
    channel channels[5];//0:r, 1:g, 2:b, 3:grey, 4:binary
    IO io;
public:
    void read_bmp(std::string &file_name);

    void decode_bmp();

    void write_bmp(std::string &file_name);

    void encode_bmp();

};


#endif //SRC_FRAME_H
