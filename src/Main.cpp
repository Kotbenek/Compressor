#include "Main.h"

#include "Huffman.h"

using namespace std;

int main(int argc, char** argv)
{        
    Huffman* h = new Huffman();
    h->count_occurences("testfile.txt");
    h->sort_nodes();
    h->construct_tree();
    h->create_codebook();
    h->create_canonical_codebook();
    h->compress_file("testfile.txt", "testfile.huffman");
    h->~Huffman();    
    
    //Exit program
    return 0;
}
