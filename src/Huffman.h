#pragma once

#include "Node.h"
#include "HuffmanCodebook.h"

#include <string>

using namespace std;

class Huffman
{
public:
	Node** nodes;
	Node* tree;
	int leaf_nodes;
	
	HuffmanCodebook* codebook;
	
	void count_occurences(string s);
	void sort_nodes();
	void construct_tree();
	void create_codebook();
	void create_canonical_codebook();
	
	Huffman();
	~Huffman();
};
