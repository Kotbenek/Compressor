#pragma once

#include <cstdint>

using namespace std;

class HuffmanCodebook
{
public:
    uint16_t size;
    uint32_t* codes;
    uint8_t* codes_length;
    uint8_t* codes_value;
    
    HuffmanCodebook(uint16_t size);
    ~HuffmanCodebook();
};
