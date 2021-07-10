#include "Main.h"

#include "Huffman.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        cout << "\r\n";
        cout << "Usage: ./compressor [-hc] [-hd] file1 file2" << "\r\n";
        
        cout << "\r\n";
        
        cout << "Options:" << "\r\n";
        cout << "    -hc" << "    " << "Compress the file1 using canonical Huffman compression" << "\r\n";
        cout << "       " << "    " << "and save the result as file2" << "\r\n";
        cout << "    -hd" << "    " << "Decompress the file1 using canonical Huffman compression" << "\r\n";
        cout << "       " << "    " << "and save the result as file2" << "\r\n";
        cout << "\r\n";
    }
    else
    {
        string file1 = argv[argc - 2];
        string file2 = argv[argc - 1];
        string operation = argv[argc - 3];
        
        if (operation == "-hc")
        {
            Huffman* h = new Huffman();
            h->count_occurences(file1);
            h->sort_nodes();
            h->construct_tree();
            h->create_codebook();
            h->create_canonical_codebook();
            h->compress_file(file1, file2);
            h->~Huffman();
        }
        else if (operation == "-hd")
        {
            Huffman* h = new Huffman();
            h->read_canonical_codebook(file1);
            h->decompress_file(file1, file2);
            h->~Huffman();
        }
        else
        {
            cout << "Bad argument: " << operation << "\r\n";
        }
    }
    
    //Exit program
    return 0;
}
