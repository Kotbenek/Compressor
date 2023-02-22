#pragma once

#include <string>

using namespace std;

class CompressionAlgorithm
{
public:
    virtual void compress_file(string file_in, string file_out) = 0;
    virtual void decompress_file(string file_in, string file_out) = 0;

    virtual ~CompressionAlgorithm() {}
};