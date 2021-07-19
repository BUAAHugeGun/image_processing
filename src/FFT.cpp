#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

#include "FFT.h"
#include "omp.h"

FFT::FFT(const FFT &rhs) {
    if (this->data != nullptr) {
        for (int i = 0; i < this->length; i++)
            delete[] this->data[i];
        delete[] this->data;
    }
    this->length = rhs.length;
    this->height = rhs.height;
    this->width = rhs.width;
    if (rhs.data == nullptr)
        return;
    this->data = new cpx *[length];
    for (int i = 0; i < length; i++)
        this->data[i] = new cpx[length];
    for (int r = 0; r < length; r++)
        for (int c = 0; c < length; c++)
            this->data[r][c] = rhs.data[r][c];
}

FFT &FFT::operator=(FFT rhs) {
    swap(*this, rhs);
    return *this;
}

void FFT::swap(FFT &a, FFT &b) {
    std::swap(a.data, b.data);
    std::swap(a.length, b.length);
    std::swap(a.height, b.height);
    std::swap(a.width, b.width);
}

uint32_t FFT::get_length() const {
    return length;
}

uint32_t FFT::get_height() const {
    return height;
}

uint32_t FFT::get_width() const {
    return width;
}


cpx cpx::operator+(const cpx &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

cpx cpx::operator-(const cpx &rhs) const {
    return {x - rhs.x, y - rhs.y};
}

cpx cpx::operator*(const cpx &rhs) const {
    return {x * rhs.x - y * rhs.y, x * rhs.y + y * rhs.x};
}

FFT::FFT(double **input, uint32_t height, uint32_t width) {
    this->height = height;
    this->width = width;
    length = 1;
    while (length < height || length < width)length <<= 1;
    if (data != nullptr) {
        for (int i = 0; i < length; i++)
            delete[] data[i];
        delete[] data;
    }
    data = new cpx *[length];
    for (int i = 0; i < length; i++)
        data[i] = new cpx[length];
    for (int r = 0; r < length; r++)
        for (int c = 0; c < length; c++)
            if (r < height && c < width)
                this->data[r][c] = cpx(input[r][c], 0.);
            else
                this->data[r][c] = cpx(0, 0);
}

void FFT::trans() {
    for (int r = 0; r < length; r++)
        for (int c = 0; c <= r; c++)
            std::swap(data[r][c], data[c][r]);
}

void FFT::FFT1D(cpx *a, int n, int rev) {
    if (n == 1) return;
    cpx a1[(n >> 1) + 1], a2[(n >> 1) + 1];
    for (int i = 0; i < (n >> 1); ++i) a1[i] = a[i << 1], a2[i] = a[i << 1 | 1];
    FFT1D(a1, n >> 1, rev);
    FFT1D(a2, n >> 1, rev);
    cpx w1(cos(2 * PI / n), -rev * sin(2 * PI / n)), w(1, 0);  //idftÓÃµÄ¸º¸ù
    for (int i = 0; i < (n >> 1); ++i, w = w * w1) {
        a[i] = a1[i] + w * a2[i];
        a[i + (n >> 1)] = a1[i] - w * a2[i];
    }
}

void FFT::FFT2D(int rev) {
    omp_set_num_threads(8);
#pragma omp parallel for
    for (int r = 0; r < length; r++)
        FFT1D(data[r], length, rev);
    this->trans();

#pragma omp parallel for
    for (int r = 0; r < length; r++)
        FFT1D(data[r], length, rev);
    trans();
}

void FFT::center() {
    uint32_t offset = length >> 1;
    for (int r = 0; r < offset; r++)
        for (int c = 0; c < length; c++) {
            std::swap(data[r][c], data[(r + offset) % length][(c + offset) % length]);
        }
}

double FFT::get_value(uint32_t r, uint32_t c) const {
    return std::sqrt(data[r][c].x * data[r][c].x + data[r][c].y * data[r][c].y) / length;
}

#pragma clang diagnostic pop