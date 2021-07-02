#pragma once

#include "Node.h"
#include "HuffmanCodebook.h"

#include <cstdint>

using namespace std;

class Sort
{    
public:
    static void sort_nodes_by_occurences(Node** a, uint16_t size);
    static void sort_huffman_codebook_by_length_then_by_value_ascending(HuffmanCodebook* hc);
};
