#include "frame.h"

channel::channel(const channel &rhs) {
    if (this->is_empty()) {
        for (int i = 0; i < height; i++)
            delete[] this->data[i];
        delete[] this->data;
    }
    this->alloc(rhs.height, rhs.width);
    this->height = rhs.height;
    this->width = rhs.width;
    for (int r = 0; r < this->height; r++)
        for (int c = 0; c < this->width; c++)
            this->data[r][c] = rhs.data[r][c];
}

channel &channel::operator=(channel rhs) {
    channel::swap(*this, rhs);
    return *this;
}

void channel::swap(channel &a, channel &b) {
    std::swap(a.data, b.data);
    std::swap(a.width, b.width);
    std::swap(a.height, b.height);
}

bool channel::is_empty() const {
    return (data == nullptr);
}

void channel::alloc(uint32_t h, uint32_t w) {
    data = new double *[h];
    for (int i = 0; i < h; i++)
        data[i] = new double[w];
    height = h, width = w;
}

void channel::set_pixel(uint32_t r, uint32_t c, double value) {
    this->data[r][c] = value;
}

double channel::get_pixel(uint32_t r, uint32_t c) const {
    return this->data[r][c];
}

FFT channel::init_fft() {
    return FFT(this->data, this->height, this->width);
}

void frame::RGB2HSI() {
    for (int r = 0; r < this->height; r++)
        for (int c = 0; c < this->width; c++) {
            double R = this->channels[CO_RED].get_pixel(r, c);
            double G = this->channels[CO_GREEN].get_pixel(r, c);
            double B = this->channels[CO_BLUE].get_pixel(r, c);
            double theta =
                    std::acos(.5 * (R - G + R - B) / std::sqrt((R - G) * (R - G) + (R - B) * (G - B) + 1e-8)) / PI *
                    180.;
            while (theta < 0)theta += 360;
            while (theta >= 360)theta -= 360;
            double H = (B <= G) ? theta : 360 - theta;
            double I = 1. / 3 * (R + G + B);
            double S = (1 - std::min(R, std::min(G, B)) / I);
            this->channels[CO_H].set_pixel(r, c, H);
            this->channels[CO_S].set_pixel(r, c, S);
            this->channels[CO_I].set_pixel(r, c, I);
        }
}

void frame::HSI2RGB() {
    for (int r = 0; r < this->height; r++)
        for (int c = 0; c < this->width; c++) {
            double H = this->channels[CO_H].get_pixel(r, c);
            double S = this->channels[CO_S].get_pixel(r, c);
            double I = this->channels[CO_I].get_pixel(r, c);
            double R, G, B;
            if (H < 120) {
                B = I * (1 - S);
                R = I * (1 + S * cos(H / 180 * PI) / cos((60 - H) / 180 * PI));
                G = 3. * I - (R + B);
            } else if (H < 240) {
                H -= 120;
                R = I * (1 - S);
                G = I * (1 + S * cos(H / 180 * PI) / cos((60 - H) / 180 * PI));
                B = 3. * I - (R + G);
            } else {
                H -= 240;
                G = I * (1 - S);
                B = I * (1 + S * cos(H / 180 * PI) / cos((60 - H) / 180 * PI));
                R = 3. * I - (B + G);
            }
            this->channels[CO_RED].set_pixel(r, c, R);
            this->channels[CO_GREEN].set_pixel(r, c, G);
            this->channels[CO_BLUE].set_pixel(r, c, B);
        }
}

frame::frame(uint32_t H, uint32_t W) {
    this->width = W;
    this->height = H;
    this->depth = 24;
    this->header_length = 54;
    this->info_header_length = 40;
    this->image_data_length = height * ((width * 3 - 1) / 4 + 1) * 4;
    this->xppm = 2834;
    this->yppm = 2834;
    this->color_used = 0;
    this->im_color_used = 0;
    for (auto &channel : this->channels)
        channel.alloc(H, W);
    io.alloc(H, W);
}

//frame::frame(const frame &rhs) {
//    this->width = rhs.width;
//    this->height = rhs.height;
//    this->depth = rhs.depth;
//    this->header_length = rhs.header_length;
//    this->info_header_length = rhs.info_header_length;
//    this->image_data_length = rhs.image_data_length;
//    this->xppm = rhs.xppm;
//    this->yppm = rhs.yppm;
//    this->color_used = rhs.color_used;
//    this->im_color_used = rhs.im_color_used;
//    this->io = rhs.io;
//    for (int i = 0; i < 7; i++)
//        this->channels[i] = rhs.channels[i];
//}

//frame &frame::operator=(const frame &rhs) {
//
//    return *this;
//}

void frame::read_bmp(std::string &file_name) {
    io.read_file(file_name);
    decode_bmp();
    std::cout << "read " + file_name + " successfully!\n\n";
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
//    printf("header length: %u\n", header_length);
    //info header length, w, h
    info_header_length = io.next_uint(4);
//    printf("info header length: %u\n", info_header_length);
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
//    printf("image depth: %u\n", depth);
    //compress
    if (io.next_uint(4) != 0) {
        printf("compression check failed!\n");
        assert(0);
    }
    //image data length
    image_data_length = io.next_uint(4);
//    printf("image data length: %u\n", image_data_length);
    if (image_data_length + header_length != io.get_file_size()) {
        printf("image data length check failed!\n");
        assert(0);
    }
    //x,y pixels per meter
    xppm = io.next_uint(4);
    yppm = io.next_uint(4);
//    printf("pixels per meter: (%u,%u)\n", xppm, yppm);
    //color used
    color_used = io.next_uint(4);
    im_color_used = io.next_uint(4);
//    printf("color used: (%u,%u)\n", color_used, im_color_used);
    //image data
    io.next_uint(header_length - 54);
    for (auto &channel:channels)
        channel.alloc(height, width);

    uint32_t width_left = ((width * 3 - 1) / 4 + 1) * 4 - 3 * width;
    for (int i = (int) height - 1; i >= 0; i--) {
        for (int j = 0; j < (int) width; j++) {
            channels[CO_BLUE].set_pixel(i, j, (double) (io.next_uint(1)));
            channels[CO_GREEN].set_pixel(i, j, (double) (io.next_uint(1)));
            channels[CO_RED].set_pixel(i, j, (double) (io.next_uint(1)));
        }
        io.next_uint(width_left);
    }
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
//    printf("header length: %u\n", 54);
    //info header length, w, h
    io.set_uint(40, 4);
//    printf("info header length: %u\n", 40);
    io.set_uint(width, 4);
    printf("image width: %u\n", width);
    io.set_uint(height, 4);
    printf("image height: %u\n", height);
    //bit plane
    io.set_uint(1, 2);
    //depth
    io.set_uint(depth, 2);
//    printf("image depth: %u\n", depth);
    //compress
    io.set_uint(0, 4);
    //image data length
    io.set_uint(image_data_length, 4);
//    printf("image data length: %u\n", image_data_length);
    //x,y pixels per meter
    io.set_uint(xppm, 4);
    io.set_uint(yppm, 4);
//    printf("pixels per meter: (%u,%u)\n", xppm, yppm);
    //color used
    io.set_uint(color_used, 4);
    io.set_uint(color_used, 4);
//    printf("color used: (%u,%u)\n", color_used, im_color_used);
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
}

void frame::write_bmp(std::string &file_name) {
    io.alloc(height, width);
    encode_bmp();
    io.write_file(file_name);
    std::cout << "write " + file_name + " successfully!\n\n";
}

void frame::read_raw(std::string &file_name) {
    io.read_file(file_name);
    height = io.next_uint(4);
    width = io.next_uint(4);
    printf("raw image height: %d, width: %d\n", height, width);
    for (auto &channel:channels)
        channel.alloc(height, width);
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (((r + c) & 1) == 0)
                channels[CO_GREEN].set_pixel(r, c, io.next_uint(1));
            else if (r & 1)
                channels[CO_BLUE].set_pixel(r, c, io.next_uint(1));
            else
                channels[CO_RED].set_pixel(r, c, io.next_uint(1));
        }
    }
    double temp;
    uint32_t cnt;
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++) {
            if (((r + c) & 1) == 0) {
                if (r & 1) {
                    temp = 0.;
                    cnt = 0;
                    if (r > 0)temp += channels[CO_RED].get_pixel(r - 1, c), cnt++;
                    if (r < height - 1)temp += channels[CO_RED].get_pixel(r + 1, c), cnt++;
                    channels[CO_RED].set_pixel(r, c, temp / cnt);

                    temp = 0.;
                    cnt = 0;
                    if (c > 0)temp += channels[CO_BLUE].get_pixel(r, c - 1), cnt++;
                    if (c < width - 1)temp += channels[CO_BLUE].get_pixel(r, c + 1), cnt++;
                    channels[CO_BLUE].set_pixel(r, c, temp / cnt);
                } else {
                    temp = 0.;
                    cnt = 0;
                    if (r > 0)temp += channels[CO_BLUE].get_pixel(r - 1, c), cnt++;
                    if (r < height - 1)temp += channels[CO_BLUE].get_pixel(r + 1, c), cnt++;
                    channels[CO_BLUE].set_pixel(r, c, temp / cnt);

                    temp = 0.;
                    cnt = 0;
                    if (c > 0)temp += channels[CO_RED].get_pixel(r, c - 1), cnt++;
                    if (c < width - 1)temp += channels[CO_RED].get_pixel(r, c + 1), cnt++;
                    channels[CO_RED].set_pixel(r, c, temp / cnt);
                }
            } else if (r & 1) {
                temp = 0.;
                cnt = 0;
                if (r > 0)temp += channels[CO_GREEN].get_pixel(r - 1, c), cnt++;
                if (r < height - 1)temp += channels[CO_GREEN].get_pixel(r + 1, c), cnt++;
                if (c > 0)temp += channels[CO_GREEN].get_pixel(r, c - 1), cnt++;
                if (c < width - 1)temp += channels[CO_GREEN].get_pixel(r, c + 1), cnt++;
                channels[CO_GREEN].set_pixel(r, c, temp / cnt);

                temp = 0.;
                cnt = 0;
                if (r > 0 && c > 0)temp += channels[CO_RED].get_pixel(r - 1, c - 1), cnt++;
                if (r > 0 && c < width - 1)temp += channels[CO_RED].get_pixel(r - 1, c + 1), cnt++;
                if (r < height - 1 && c > 0)temp += channels[CO_RED].get_pixel(r + 1, c - 1), cnt++;
                if (r < height - 1 && c < width - 1)temp += channels[CO_RED].get_pixel(r + 1, c + 1), cnt++;
                channels[CO_RED].set_pixel(r, c, temp / cnt);
            } else {
                temp = 0.;
                cnt = 0;
                if (r > 0)temp += channels[CO_GREEN].get_pixel(r - 1, c), cnt++;
                if (r < height - 1)temp += channels[CO_GREEN].get_pixel(r + 1, c), cnt++;
                if (c > 0)temp += channels[CO_GREEN].get_pixel(r, c - 1), cnt++;
                if (c < width - 1)temp += channels[CO_GREEN].get_pixel(r, c + 1), cnt++;
                channels[CO_GREEN].set_pixel(r, c, temp / cnt);

                temp = 0.;
                cnt = 0;
                if (r > 0 && c > 0)temp += channels[CO_BLUE].get_pixel(r - 1, c - 1), cnt++;
                if (r > 0 && c < width - 1)temp += channels[CO_BLUE].get_pixel(r - 1, c + 1), cnt++;
                if (r < height - 1 && c > 0)temp += channels[CO_BLUE].get_pixel(r + 1, c - 1), cnt++;
                if (r < height - 1 && c < width - 1)temp += channels[CO_BLUE].get_pixel(r + 1, c + 1), cnt++;
                channels[CO_BLUE].set_pixel(r, c, temp / cnt);
            }
        }
    std::cout << "read " + file_name + " successfully!\n\n";
}

void frame::write_raw(std::string &file_name) {
    io.alloc(8 + (height * width));
    io.set_uint(height, 4);
    io.set_uint(width, 4);
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++) {
            if (((r + c) & 1) == 0)
                io.set_uint(channels[CO_GREEN].get_pixel(r, c), 1);
            else if (r & 1)
                io.set_uint(channels[CO_BLUE].get_pixel(r, c), 1);
            else
                io.set_uint(channels[CO_RED].get_pixel(r, c), 1);
        }

    io.write_file(file_name);
    std::cout << "write " + file_name + " successfully!\n\n";
}

inline double max_add(double a, double b, double MAX) {
    return std::min(a + b, MAX);
}

inline double max_add_avg(double a, double b, double MAX) {
    return std::min((a + b) / 2, MAX);
}

frame frame::operator+(const frame &rhs) const {
    uint32_t H = std::max(this->height, rhs.height);
    uint32_t W = std::max(this->width, rhs.width);
    frame ret = frame(H, W);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            for (int i = 0; i < 3; i++) {
                double a = (r < this->height && c < this->width) ? this->channels[i].get_pixel(r, c) : 0.;
                double b = (r < rhs.height && c < rhs.width) ? rhs.channels[i].get_pixel(r, c) : 0.;
                ret.channels[i].set_pixel(r, c, max_add(a, b, 255.));
            }
        }
    }
    return ret;
}

frame frame::operator-(const frame &rhs) const {
    uint32_t H = std::max(this->height, rhs.height);
    uint32_t W = std::max(this->width, rhs.width);
    frame ret = frame(H, W);
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            for (int i = 0; i < 3; i++) {
                double a = (r < this->height && c < this->width) ? this->channels[i].get_pixel(r, c) : 0.;
                double b = (r < rhs.height && c < rhs.width) ? rhs.channels[i].get_pixel(r, c) : 0.;
                ret.channels[i].set_pixel(r, c, max_add_avg(a, b, 255.));
            }
        }
    }
    return ret;
}

frame frame::translation(uint32_t dir, uint32_t offset) const {
    uint32_t H = this->height;
    uint32_t W = this->width;
    assert(dir < 4);
    if (dir & 1)H += offset;
    else W += offset;
    frame ret = frame(H, W);
    for (int r = 0; r < H; r++)
        for (int c = 0; c < W; c++) {
            for (int i = 0; i < 3; i++)
                if (dir == 0) {
                    if (c < offset)ret.channels[i].set_pixel(r, c, 0.);
                    else ret.channels[i].set_pixel(r, c, this->channels[i].get_pixel(r, c - offset));
                } else if (dir == 1) {
                    if (r >= this->height)ret.channels[i].set_pixel(r, c, 0);
                    else ret.channels[i].set_pixel(r, c, this->channels[i].get_pixel(r, c));
                } else if (dir == 2) {
                    if (c >= this->width)ret.channels[i].set_pixel(r, c, 0);
                    else ret.channels[i].set_pixel(r, c, this->channels[i].get_pixel(r, c));
                } else if (dir == 3) {
                    if (r < offset)ret.channels[i].set_pixel(r, c, 0);
                    else ret.channels[i].set_pixel(r, c, this->channels[i].get_pixel(r - offset, c));
                }
        }
    return ret;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

frame frame::rotate(double angle) const {
    angle = angle / 180 * PI;
    double cos_angle = std::cos(-angle), sin_angle = std::sin(-angle);
    frame ret = frame(this->height, this->width);
    uint32_t H = this->height;
    uint32_t W = this->width;
    double centerR = .5 * H;
    double centerC = .5 * W;

    omp_set_num_threads(16);
#pragma omp parallel for
    for (int r = 0; r < this->height; r++) {
        for (int c = 0; c < this->width; c++) {
            double R = r - centerR;
            double C = c - centerC;
            double pR = R * cos_angle - C * sin_angle + centerR;
            double pC = R * sin_angle + C * cos_angle + centerC;
            if (pR < 0 || pR > H - 1 || pC < 0 || pC > W - 1) {
                for (int i = 0; i < 3; i++)
                    ret.channels[i].set_pixel(r, c, 0);
            } else {
                uint32_t prl = std::floor(pR), prr = std::ceil(pR);
                uint32_t pcl = std::floor(pC), pcr = std::ceil(pC);
                std::vector<uint32_t> points_R = {(uint32_t) prl, (uint32_t) prl,
                                                  (uint32_t) prr, (uint32_t) prr};
                std::vector<uint32_t> points_C = {(uint32_t) pcl, (uint32_t) pcr,
                                                  (uint32_t) pcl, (uint32_t) pcr};
                for (int i = 0; i < 3; i++) {
                    std::vector<double> points_V;
                    for (int j = 0; j < 4; j++)
                        points_V.push_back(this->channels[i].get_pixel(points_R[j], points_C[j]));
                    double value_up = (pR - prl) * (points_V[1] - points_V[0]) + points_V[0];
                    double value_down = (pR - prl) * (points_V[3] - points_V[2]) + points_V[2];
                    double value = (pC - pcl) * (value_down - value_up) + value_up;
                    ret.channels[i].set_pixel(r, c, value);
                }

            }
        }
    }
    return ret;
}

#pragma clang diagnostic pop

frame frame::flip(bool dir) const {
    frame ret = frame(this->height, this->width);
    if (dir == 0) {
        for (int i = 0; i < 3; i++)
            for (int r = 0; r < this->height; r++)
                for (int c = 0; c < this->width; c++) {
                    ret.channels[i].set_pixel(r, this->width - c - 1, this->channels[i].get_pixel(r, c));
                }
    } else {
        for (int i = 0; i < 3; i++)
            for (int r = 0; r < this->height; r++)
                for (int c = 0; c < this->width; c++) {
                    ret.channels[i].set_pixel(this->height - r - 1, c, this->channels[i].get_pixel(r, c));
                }
    }
    return ret;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

frame frame::scale(double scale_x, double scale_y) const {
    assert(scale_x > 0 && scale_y > 0);
    auto H = (uint32_t) std::floor(this->height * scale_y);
    auto W = (uint32_t) std::floor(this->width * scale_x);
    while (H / scale_y > this->height - 1)H--;
    while (W / scale_x > this->width - 1)W--;
    frame ret = frame(H, W);

    omp_set_num_threads(16);
#pragma omp parallel for
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            double pR = r / scale_y;
            double pC = c / scale_x;
            uint32_t prl = std::floor(pR), prr = std::ceil(pR);
            uint32_t pcl = std::floor(pC), pcr = std::ceil(pC);
            std::vector<uint32_t> points_R = {(uint32_t) prl, (uint32_t) prl,
                                              (uint32_t) prr, (uint32_t) prr};
            std::vector<uint32_t> points_C = {(uint32_t) pcl, (uint32_t) pcr,
                                              (uint32_t) pcl, (uint32_t) pcr};
            for (int i = 0; i < 3; i++) {
                std::vector<double> points_V;
                for (int j = 0; j < 4; j++)
                    points_V.push_back(this->channels[i].get_pixel(points_R[j], points_C[j]));
                double value_up = (pR - prl) * (points_V[1] - points_V[0]) + points_V[0];
                double value_down = (pR - prl) * (points_V[3] - points_V[2]) + points_V[2];
                double value = (pC - pcl) * (value_down - value_up) + value_up;
                ret.channels[i].set_pixel(r, c, value);
            }
        }
    }
    return ret;
}

#pragma clang diagnostic pop

frame frame::reverse() const {
    frame ret = frame(this->height, this->width);
    for (int i = 0; i < 3; i++)
        for (int r = 0; r < this->height; r++)
            for (int c = 0; c < this->width; c++)
                ret.channels[i].set_pixel(r, c, 255. - this->channels[i].get_pixel(r, c));
    return ret;
}

frame frame::he() const {
    frame ret = *this;
    ret.RGB2HSI();
    uint32_t pixels = ret.height * ret.width;
    uint32_t Min = 255, Max = 0;
    double p[300];
    memset(p, 0, sizeof(p));
    for (int r = 0; r < ret.height; r++)
        for (int c = 0; c < ret.width; c++) {
            auto value = (uint32_t) ret.channels[CO_I].get_pixel(r, c);
            Max = std::max(value, Max);
            Min = std::min(value, Min);
            p[value]++;
        }
    for (uint32_t i = Min + 1; i <= Max; i++)
        p[i] += p[i - 1];
    for (uint32_t i = Min; i <= Max; i++)
        p[i] /= pixels;
    for (int r = 0; r < ret.height; r++)
        for (int c = 0; c < ret.width; c++)
            ret.channels[CO_I].set_pixel(r, c, p[(uint32_t) ret.channels[CO_I].get_pixel(r, c)] * (Max - Min) + Min);
    ret.HSI2RGB();
    return ret;
}

frame frame::image_fft() {
    for (int i = 0; i < 3; i++) {
        fft[i] = channels[i].init_fft();
        fft[i].FFT2D(1);
        fft[i].center();
    }
    uint32_t length = fft[0].get_length();
    frame ret = frame(length, length);
    for (int i = 0; i < 3; i++)ret.fft[i] = this->fft[i];
    for (int r = 0; r < length; r++)
        for (int c = 0; c < length; c++)
            for (int i = 0; i < 3; i++)
                ret.channels[i].set_pixel(r, c, fft[i].get_value(r, c));
    return ret;
}

frame frame::image_ifft() {
    for (auto &i : fft) {
        i.center();
        i.FFT2D(-1);
    }
    uint32_t length = fft[0].get_length();
    frame ret = frame(fft[0].get_height(), fft[0].get_width());
    for (int r = 0; r < ret.height; r++)
        for (int c = 0; c < ret.width; c++)
            for (int i = 0; i < 3; i++)
                ret.channels[i].set_pixel(r, c, fft[i].get_value(r, c) / length);
    return ret;
}

frame frame::conv(double kernel[3][3], uint32_t k_size) {
    uint32_t H = this->height - k_size + 1;
    uint32_t W = this->width - k_size + 1;
    frame ret = frame(H, W);
    for (uint32_t r = 0; r < ret.height; r++)
        for (uint32_t c = 0; c < ret.width; c++) {
            for (int i = 0; i < 3; i++) {
                double temp = 0.;
                for (uint32_t rr = 0; rr < k_size; rr++)
                    for (uint32_t cc = 0; cc < k_size; cc++) {
                        temp += kernel[rr][cc] * this->channels[i].get_pixel(r + rr, c + cc);
                    }
                ret.channels[i].set_pixel(r, c, temp);
            }
        }
    return ret;
}

frame frame::FD(const std::vector<int> &x, const std::vector<int> &y, uint32_t N) {
    uint32_t T = x.size();
    std::vector<double> xx(T), ax(N + 1), bx(N + 1);
    std::vector<double> yy(T), ay(N + 1), by(N + 1);
    double omega = 2. * PI / T;
    for (int i = 0; i <= N; i++) {
        ax[i] = bx[i] = ay[i] = by[i] = 0;
        for (int t = 0; t < T; t++) {
            ax[i] += 1. / T * x[t] * std::cos(omega * i * t);
            bx[i] += 1. / T * x[t] * std::sin(omega * i * t);
            ay[i] += 1. / T * y[t] * std::cos(omega * i * t);
            by[i] += 1. / T * y[t] * std::sin(omega * i * t);

        }
    }
    frame ret = frame(128, 128);
    for (int r = 0; r < 128; r++)
        for (int c = 0; c < 128; c++)
            for (int i = 0; i < 3; i++)
                ret.channels[i].set_pixel(r, c, 0);
    uint32_t off;
    for (int t = 0; t < T; t++) {
        xx[t] = ax[0];
        yy[t] = ay[0];
        for (int i = 1; i <= N; i++) {
            xx[t] += ax[i] * std::cos(i * omega * t) + bx[i] * std::sin(i * omega * t);
            yy[t] += ay[i] * std::cos(i * omega * t) + by[i] * std::sin(i * omega * t);
        }
        for (int i = 0; i < 3; i++)
            ret.channels[i].set_pixel(round(xx[t]) + (N > 32 ? 0 : 32), round(yy[t]) + (N > 32 ? 0 : 32), 255);
    }
    return ret;
}