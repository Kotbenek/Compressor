#include "Huffman.h"

#include "Node.h"
#include "Sort.h"
#include "HuffmanCodebook.h"

#include <string>
#include <iostream>
#include <queue>
#include <bitset>

using namespace std;

void Huffman::count_occurences(string s)
{
	//Count byte occurences
	for (uint32_t i = 0; i < s.size(); i++)
	{
		nodes[(uint32_t)s[i]]->occurences++;	
	}
}

void Huffman::sort_nodes()
{
	//Sort nodes descending by occurences
	Sort::sort_nodes_by_occurences(nodes, 256);
}

void Huffman::construct_tree()
{
	//Create priority queue with custom comparator (sort by occurences ascending, then by id ascending)
	auto comparator = []( Node* a, Node* b ) 
	{ 
		if (a->occurences > b->occurences) return true;
		if (a->occurences == b->occurences) return a->id > b->id;
		return false;
	};
	priority_queue<Node*, vector<Node*>, decltype(comparator)> queue(comparator);
	
	//Fill priority queue with data
	for (uint16_t i = 0; i < 256; i++)
	{
		if (!nodes[i]->occurences) 
		{
			leaf_nodes = i;
			break;
		}
		queue.push(nodes[i]);
	}
	
	//Process while the queue is not empty
	while (queue.size() > 1)
	{
		//Remove two nodes of lowest probability from queue
		Node* n1 = queue.top();
		queue.pop();
		Node* n2 = queue.top();
		queue.pop();
		
		//Create new node using acquired two nodes
		Node* n = new Node(-1, false);
		n->left = n1;
		n->right = n2;
		n->occurences = n1->occurences + n2->occurences;
		n1->parent = n;
		n2->parent = n;
		
		//Add new node to queue
		queue.push(n);
	}
	
	//Store constructed tree
	tree = queue.top();
	queue.pop();
}

void Huffman::create_codebook()
{
	//Initialize codebook
	codebook = new HuffmanCodebook(leaf_nodes);
	
	//For every byte to encode
	for (uint16_t i = 0; i < leaf_nodes; i++)
	{
		//Create codeword
		uint32_t codeword = 0;
		uint8_t codeword_length = 0;
		//Start from MSB
		uint8_t position = 31;
		
		//Get node
		Node* n = nodes[i];
		
		//Do while node is not root
		do
		{			
			//If node is right leaf of parent, set bit at this position
			if (n->parent->right == n) codeword |= (1 << position);
						
			//Move position
			position--;
			
			//Move to parent
			n = n->parent;
			
			//Increment codeword length
			codeword_length++;
			
		} while(n->parent != NULL);
		
		//Store the codeword in the codebook
		codebook->codes[i] = 0;
		for (uint8_t j = 31, k = 0; j > 31 - codeword_length; j--)
		{
			codebook->codes[i] |= (((codeword & (1 << j)) >> j) << k++);
		}
		codebook->codes_length[i] = codeword_length;
		codebook->codes_value[i] = nodes[i]->id;
	}
}

void Huffman::create_canonical_codebook()
{
	//Sort the codebook ascending by codeword length and then by codeword value
	Sort::sort_huffman_codebook_by_length_then_by_value_ascending(codebook);
	
	//All codeword lengths stay the same
	//Set the first symbol to 0
	codebook->codes[0] = 0;
	for (uint32_t i = 1; i < codebook->size; i++)
	{
		//For each symbol starting from the second one, assign next binary number
		//If the codeword is longer than the previous one, append zeros (left shift)
		codebook->codes[i] = (codebook->codes[i - 1] + 1) << (codebook->codes_length[i] - codebook->codes_length[i - 1]);
	}
	
	//Debug: print codebook
	for (uint32_t i = 0; i < codebook->size; i++)
	{		
		cout << (char)codebook->codes_value[i] << " ";
		for (int8_t j = codebook->codes_length[i] - 1; j >= 0; j--)
		{
			cout << bitset<32>(codebook->codes[i])[j];
		}
		cout << "\r\n";
	}
}

Huffman::Huffman()
{
	this->nodes = new Node*[256];
	for (uint16_t i = 0; i < 256; i++)
	{
		this->nodes[i] = new Node(i, true);
	}
	this->leaf_nodes = 0;
	this->tree = NULL;
	this->codebook = NULL;
}

Huffman::~Huffman()
{
	tree->~Node();
	delete[] nodes;
	delete codebook;
}
