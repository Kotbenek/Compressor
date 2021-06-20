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
	for (int i = 0; i < (int)s.size(); i++)
	{
		nodes[(int)s[i]]->occurences++;	
	}
}

void Huffman::sort_nodes()
{
	//Sort nodes descending by occurences
	Sort::bubble_sort(nodes, 256);
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
	for (int i = 0; i < 256; i++)
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
	for (int i = 0; i < leaf_nodes; i++)
	{
		//Create codeword
		uint32_t codeword = 0;
		uint8_t codeword_length = 0;
		//Start from MSB
		int position = 31;
		
		//Get node
		Node* n = nodes[i];
		
		//Do while node is not root
		do
		{			
			//If node is right leaf of parent, set bit at this position
			if (n->parent->right == n) codeword |= (1UL << position);
						
			//Move position
			position--;
			
			//Move to parent
			n = n->parent;
			
			//Increment codeword length
			codeword_length++;
			
		} while(n->parent != NULL);
		
		//Store the codeword in the codebook
		codebook->codes[i] = 0;
		for (int j = 31, k = 0; j > 31 - codeword_length; j--)
		{
			codebook->codes[i] |= (((codeword & (1 << j)) >> j) << k++);
		}
		codebook->codes_length[i] = codeword_length;
		codebook->codes_value[i] = nodes[i]->id;		
		
		//Debug: print codebook
		cout << (char)nodes[i]->id << " ";
		for (int j = 32 - codeword_length; j < 32; j++)
		{
			cout << ((codeword & (1 << j)) >> j);
		}
		cout << " ";
		for (int j = codeword_length - 1; j >= 0; j--)
		{
			cout << bitset<32>(codebook->codes[i])[j];
		}
		cout << "\r\n";
	}
}

Huffman::Huffman()
{
	this->nodes = new Node*[256];
	for (int i = 0; i < 256; i++)
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
