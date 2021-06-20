#pragma once

#include <cstdint>

using namespace std;

class HuffmanCodebook
{
public:
	uint32_t size;
	uint32_t* codes;
	uint8_t* codes_length;
	uint8_t* codes_value;
	
	HuffmanCodebook(uint32_t size);
	~HuffmanCodebook();
};
