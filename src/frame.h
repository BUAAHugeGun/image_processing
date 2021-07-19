#ifndef SRC_FRAME_H
#define SRC_FRAME_H

#include <bits/stdc++.h>
#include "IO.h"
#include "omp.h"
#include "FFT.h"

#define CO_RED 0
#define CO_GREEN 1
#define CO_BLUE 2
#define CO_GREY 3
#define CO_BINARY 4
#define CO_H 5
#define CO_S 6
#define CO_I 7


class channel {
    double **data = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
public:
    channel() = default;

    channel(const channel &rhs);

    channel &operator=(channel rhs);

    static void swap(channel &a, channel &b);

    bool is_empty() const;

    void alloc(uint32_t h, uint32_t w);

    void set_pixel(uint32_t r, uint32_t c, double value);

    double get_pixel(uint32_t r, uint32_t c) const;

    FFT init_fft();

    ~channel() {
        if (data != nullptr) {
            for (int i = 0; i < height; i++)
                delete[] data[i];
            delete[] data;
        }
    }
};

class frame {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 24;
    uint32_t header_length = 54;
    uint32_t info_header_length = 40;
    uint32_t image_data_length = 0;
    uint32_t xppm = 2834;
    uint32_t yppm = 2834;
    uint32_t color_used = 0;
    uint32_t im_color_used = 0;
    channel channels[8];//0:r, 1:g, 2:b, 3:grey, 4:binary, 5 y/h, 6 u/s,7 v/i
    IO io;
    FFT fft[3];

    void RGB2HSI();

    void HSI2RGB();

public:
    void RGB2YUV();

    void YUV2RGB();

    frame() = default;

    frame(uint32_t height, uint32_t width);

//    frame(const frame &rhs);

//    frame &operator=(const frame &rhs);

    void read_bmp(std::string &file_name);

    void decode_bmp();

    void write_bmp(std::string &file_name);

    void encode_bmp();

    void read_raw(std::string &file_name);

    void write_raw(std::string &file_name);

    frame operator+(const frame &rhs) const;

    frame operator-(const frame &rhs) const;

    frame translation(uint32_t dir, uint32_t offset) const;//0 right, 1 up, 2 left, 3 down

    frame rotate(double angle) const;

    frame flip(bool dir) const;//0 x, 1 y

    frame scale(double scale_x, double scale_y) const;

    frame reverse() const;

    frame he() const;

    frame image_fft();

    frame image_ifft();

    frame conv(double kernel[3][3], uint32_t k_size);

    static frame FD(const std::vector<int> &x, const std::vector<int> &y, uint32_t N);
};


#endif //SRC_FRAME_H
