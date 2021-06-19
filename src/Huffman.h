#pragma once
#include <string>

#include "Node.h"

using namespace std;

class Huffman
{
public:
	Node** nodes;
	Node* tree;
	int leaf_nodes;
	
	void count_occurences(string s);
	void sort_nodes();
	void construct_tree();
	void create_codebook();
	
	Huffman();
	~Huffman();
};
