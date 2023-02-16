#include "Main.h"

#include "Huffman.h"
#include "LZ77.h"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        cout << "\r\n";

        cout << "Usage: ./compressor -c|-d algorithm file1 file2" << "\r\n";
        cout << "\r\n";

        cout << "Options:" << "\r\n";
        cout << "     -c" << "    " << "Compress the file1 using specified algorithm" << "\r\n";
        cout << "       " << "    " << "and save the result as file2" << "\r\n";
        cout << "     -d" << "    " << "Decompress the file1 using specified algorithm" << "\r\n";
        cout << "       " << "    " << "and save the result as file2" << "\r\n";
        cout << "\r\n";

        cout << "Supported algorithms:" << "\r\n";
        cout << "huffman" << "    " << "Canonical Huffman" << "\r\n";
        cout << "lz77" << "       " << "Lempel-Ziv 77" << "\r\n";
        cout << "\r\n";
    }
    else
    {
        string operation = argv[1];
        string algorithm = argv[2];
        string file1 = argv[3];
        string file2 = argv[4];

        if (operation == "-c")
        {
            if (algorithm == "huffman")
            {
                Huffman* h = new Huffman();
                h->compress_file(file1, file2);
                h->~Huffman();
            }
            else if (algorithm == "lz77")
            {
                LZ77* lz77 = new LZ77();
                lz77->compress_file(file1, file2);
                lz77->~LZ77();
            }
            else
            {
                bad_argument(algorithm);

                //Exit program
                return 1;
            }
        }
        else if (operation == "-d")
        {
            if (algorithm == "huffman")
            {
                Huffman* h = new Huffman();
                h->decompress_file(file1, file2);
                h->~Huffman();
            }
            else if (algorithm == "lz77")
            {
                LZ77* lz77 = new LZ77();
                lz77->decompress_file(file1, file2);
                lz77->~LZ77();
            }
            else
            {
                bad_argument(algorithm);

                //Exit program
                return 1;
            }
        }
        else
        {
            bad_argument(operation);

            //Exit program
            return 1;
        }
    }

    //Exit program
    return 0;
}

void bad_argument(string s)
{
    cout << "Bad argument: " << s << "\r\n";
}