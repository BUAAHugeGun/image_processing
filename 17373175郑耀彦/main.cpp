#include <iostream>
#include "frame.h"
#include "IO.h"

double roberts1[3][3] = {{-1, 0, 0},
                         {0,  1, 0},
                         {0,  0, 0}};
double roberts2[3][3] = {{0, -1, 0},
                         {1, 0,  0},
                         {0, 0,  0}};
double prewitt1[3][3] = {{-1, 0, 1},
                         {-1, 0, 1},
                         {-1, 0, 1}};
double prewitt2[3][3] = {{-1, 0, 1},
                         {-1, 0, 1},
                         {-1, 0, 1}};
double sobel1[3][3] = {{-1, 0, 1},
                       {-2, 0, 2},
                       {-1, 0, 1}};
double sobel2[3][3] = {{-1, -2, -1},
                       {0,  0,  0},
                       {1,  2,  1}};
double laplacian[3][3] = {{-1, -1, -1},
                          {-1, 8,  -1},
                          {-1, -1, -1}};

int main() {
    std::srand(time(0));
    //读图
    frame A, B, C, D;
    std::string file_name = "A.bmp";
    A.read_bmp(file_name);
    file_name = "B.bmp";
    B.read_bmp(file_name);
    file_name = "C.bmp";
    C.read_bmp(file_name);
    file_name = "D.bmp";
    D.read_bmp(file_name);

    //将A.bmp转为raw格式
    file_name = "A.raw";
    A.write_raw(file_name);
    //读取A.raw并转为bmp格式（A1.bmp）
    frame A_raw;
    A_raw.read_raw(file_name);
    file_name = "A1.bmp";
    A_raw.write_bmp(file_name);

    //图像加法(A+B)
    file_name = "A+B.bmp";
    (A + B).write_bmp(file_name);
    //图像融合((A+B)/2)
    file_name = "A+B(avg).bmp";
    (A - B).write_bmp(file_name);

    //图像求反
    file_name = "A_rev.bmp";
    A.reverse().write_bmp(file_name);

    //图像旋转(随机角度)
    file_name = "A_rotate.bmp";
    double angle = 1. * rand() / RAND_MAX * 360;
    A.rotate(angle).write_bmp(file_name);

    //图像翻转(随机水平/竖直)
    file_name = "A_flip.bmp";
    A.flip(rand() % 2).write_bmp(file_name);

    //图像平移(随机方向、随机距离)
    file_name = "A_translation.bmp";
    uint32_t dir = rand() % 4;
    A.translation(dir, rand() % 200 + 100).write_bmp(file_name);

    //图像拉伸放缩
    file_name = "A_scale.bmp";
    double scalex = 1. * rand() / RAND_MAX * 2 + 0.5, scaley = 1. * rand() / RAND_MAX * 2 + 0.5;
    A.scale(scalex, scaley).write_bmp(file_name);

    //直方图均衡化(先转为HSI，对I进行直方图均衡，再转回RGB)
    file_name = "A_HE.bmp";
    A.he().write_bmp(file_name);

    //FFT(A,B,C,D)，反变换
    file_name = "A_FFT.bmp";
    frame A_FFT = A.image_fft();
    A_FFT.write_bmp(file_name);
    file_name = "A_IFFT.bmp";
    A_FFT.image_ifft().write_bmp(file_name);

    file_name = "B_FFT.bmp";
    frame B_FFT = B.image_fft();
    B_FFT.write_bmp(file_name);
    file_name = "B_IFFT.bmp";
    B_FFT.image_ifft().write_bmp(file_name);

    file_name = "C_FFT.bmp";
    frame C_FFT = C.image_fft();
    C_FFT.write_bmp(file_name);
    file_name = "C_IFFT.bmp";
    C_FFT.image_ifft().write_bmp(file_name);

    file_name = "D_FFT.bmp";
    frame D_FFT = D.image_fft();
    D_FFT.write_bmp(file_name);
    file_name = "D_IFFT.bmp";
    D_FFT.image_ifft().write_bmp(file_name);

    //平移后的D_FFT
    file_name = "D_FFT_translation.bmp";
    D.translation(0, 200).image_fft().write_bmp(file_name);

    //傅里叶描述子(1,4,16,64)
    std::vector<int> x(64), y(64);
    for (int i = 0; i < 16; i++)x[i] = 0, y[i] = i * 4;
    for (int i = 0; i < 16; i++)x[i + 16] = i * 4, y[i + 16] = 64;
    for (int i = 0; i < 16; i++)x[i + 32] = 64, y[i + 32] = 64 - i * 4;
    for (int i = 0; i < 16; i++)x[i + 48] = 64 - i * 4, y[i + 48] = 0;
    for (int i = 1; i <= 32; i *= 2) {
        file_name = "FD_" + std::to_string(i) + ".bmp";
        frame::FD(x, y, i).write_bmp(file_name);
    }

    //各种算子
    file_name = "A_roberts.bmp";
    (A.conv(roberts1, 2) + A.conv(roberts2, 2)).write_bmp(file_name);
    file_name = "A_sobel.bmp";
    (A.conv(sobel1, 3) + A.conv(sobel2, 3)).write_bmp(file_name);
    file_name = "A_prewitt.bmp";
    (A.conv(prewitt1, 3) + A.conv(prewitt2, 3)).write_bmp(file_name);
    file_name = "A_laplacian.bmp";
    (A.conv(laplacian, 3)).write_bmp(file_name);

//    frame ll, bq;
//    ll.read_bmp(file_name);
//    file_name = "..\\..\\bq.bmp";
//    bq.read_bmp(file_name);
//    frame f = bq.translation(3,1000) + ll;
//    frame f = ll.rotate(20);
//    frame f = ll.flip(1).rotate(180);
//    frame f=ll.scale(5,2);
//    frame f = ll.reverse();
//    frame f = ll.he();

//    frame f=ll.image_fft();
//    f=f.image_ifft();

//    frame ff=ll;
//    std::string output_name = "..\\..\\out.raw";
//    ff.write_raw(output_name);
//    frame f;
//    f.read_raw(output_name);

//    frame f = ll.conv(roberts1, 2) - ll.conv(roberts2, 2);
//    frame f = ll.conv(sobel1, 3) - ll.conv(sobel2, 3);
//    frame f = ll.conv(prewitt1, 3) - ll.conv(prewitt2, 3);
//    frame f = ll.conv(laplacian, 3);

//
//    frame f = frame::FD(x, y, 1);
//    std::string output_name = "..\\..\\out.bmp";
//    f.write_bmp(output_name);
    system("pause");
    return 0;
}
