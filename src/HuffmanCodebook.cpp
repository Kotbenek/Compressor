#include "HuffmanCodebook.h"

#include <cstdint>

using namespace std;

HuffmanCodebook::HuffmanCodebook(uint16_t size)
{
	this->size = size;
	codes = new uint32_t[size];
	codes_length = new uint8_t[size];
	codes_value = new uint8_t[size];
}

HuffmanCodebook::~HuffmanCodebook()
{
	delete[] codes;
	delete[] codes_length;
	delete[] codes_value;
}
