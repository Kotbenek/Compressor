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
	
	Huffman();
	~Huffman();
	
private:
	const int BUFFER_SIZE = 4096;
};
