#ifndef SRC_FFT_H
#define SRC_FFT_H

#include <bits/stdc++.h>
#define PI 3.1415926535

struct cpx {
    double x, y;

    cpx() = default;

    cpx(double x, double y) : x(x), y(y) {}

    cpx operator+(const cpx &rhs) const;

    cpx operator-(const cpx &rhs) const;

    cpx operator*(const cpx &rhs) const;

};

class FFT {
    cpx **data = nullptr;
    uint32_t length = 1;
    uint32_t height = 0;
    uint32_t width = 0;
public:
    FFT() = default;

    FFT(const FFT &rhs);

    FFT(double **input, uint32_t height, uint32_t width);

    FFT &operator=(FFT rhs);

    static void swap(FFT &a, FFT &b);

    uint32_t get_length() const;

    uint32_t get_height() const;

    uint32_t get_width() const;

    void trans();

    void FFT1D(cpx *a, int n, int rev = 1);

    void FFT2D(int rev = 1);

    void center();

    double get_value(uint32_t r, uint32_t c) const;

    ~FFT() {
        if (data != nullptr) {
            for (int i = 0; i < length; i++)
                delete[] data[i];
            delete[] data;
        }
    }

};

#endif //SRC_FFT_H
