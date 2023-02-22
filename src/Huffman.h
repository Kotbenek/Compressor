#pragma once

#include "CompressionAlgorithm.h"
#include "Node.h"
#include "HuffmanCodebook.h"

#include <string>
#include <cstdint>

using namespace std;

class Huffman : public CompressionAlgorithm
{
public:
    /*
    File structure:
    [size of dictionary - 1] (8-bit value)
    [size of original file] (56-bit value)
    [number of symbols with codeword length of 1...n] (multiple 8-bit values)
    [codeword values sorted in increasing order by codeword length and then by codeword value] (multiple 8-bit values)
    [compressed data]
    */
    void compress_file(string file_in, string file_out);
    void decompress_file(string file_in, string file_out);

    Huffman();
    ~Huffman();

private:
    const int BUFFER_SIZE = 4096;

    Node** nodes;
    Node* tree;
    uint16_t leaf_nodes;
    uint16_t compressed_data_starts_at;
    uint64_t original_file_size;

    HuffmanCodebook* codebook;

    void count_occurences(string file);
    void sort_nodes();
    void construct_tree();
    void create_codebook();
    void create_canonical_codebook();
    void read_canonical_codebook(string file);

    void compress(string file_in, string file_out);
    void decompress(string file_in, string file_out);
};
