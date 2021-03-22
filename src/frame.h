#ifndef HW_FRAME_H
#define HW_FRAME_H

#include <bits/stdc++.h>
#include "IO.h"

class frame {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    char *buffer = nullptr;
    IO io;
public:
    void read_bmp(std::string &file_name);

    void decode_bmp();

};


#endif //HW_FRAME_H
