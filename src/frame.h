#ifndef SRC_FRAME_H
#define SRC_FRAME_H

#include <bits/stdc++.h>
#include "IO.h"

class frame {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    uint32_t header_length;
    uint32_t info_header_length;
    char *buffer = nullptr;
    IO io;
public:
    void read_bmp(std::string &file_name);

    void decode_bmp();

};


#endif //SRC_FRAME_H
