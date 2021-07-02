#pragma once

#include "Node.h"
#include "HuffmanCodebook.h"

#include <string>
#include <cstdint>

using namespace std;

class Huffman
{
public:
    Node** nodes;
    Node* tree;
    uint16_t leaf_nodes;
    
    HuffmanCodebook* codebook;
    
    void count_occurences(string file);
    void sort_nodes();
    void construct_tree();
    void create_codebook();
    void create_canonical_codebook();
    
    /*
    File structure:
    [size of dictionary - 1] (8-bit value)
    [size of original file] (56-bit value)
    [number of symbols with codeword length of 1...n] (multiple 8-bit values)
    [codeword symbols sorted in increasing order by codeword length and then by codeword value] (multiple 8-bit values)
    [compressed data]
    */
    void compress_file(string file_in, string file_out);
    
    Huffman();
    ~Huffman();
    
private:
    const int BUFFER_SIZE = 4096;
};
