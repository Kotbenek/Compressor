#include "Main.h"

#include "Huffman.h"

using namespace std;

int main(int argc, char** argv)
{		
	Huffman* h = new Huffman();
	h->count_occurences("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
	h->sort_nodes();
	h->construct_tree();
	h->create_codebook();
	h->create_canonical_codebook();
	h->~Huffman();	
	
	//Exit program
	return 0;
}
