#include <iostream>
#include "frame.h"
#include "IO.h"

int main() {
    frame f;
    std::string file_name = "..\\..\\test.bmp";
    f.read_bmp(file_name);
    std::string output_name = "..\\..\\out.bmp";
    f.write_bmp(output_name);
    return 0;
}
