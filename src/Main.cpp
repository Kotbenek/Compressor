#include "Main.h"

#include "CompressionAlgorithm.h"
#include "Huffman.h"
#include "LZ77.h"

#include <string>
#include <iostream>
#include <cstring>
#include <getopt.h>

using namespace std;

int main(int argc, char** argv)
{
    int compress = -1;
    CompressionAlgorithm* algorithm = NULL;
    string file_in;
    string file_out;

    int c;

    while (1)
    {
        static struct option long_options[] =
        {
            {"compress",   no_argument,       0, 'c'},
            {"decompress", no_argument,       0, 'd'},
            {"algorithm",  required_argument, 0, 'a'},
            {"input",      required_argument, 0, 'i'},
            {"output",     required_argument, 0, 'o'},
            {"help",       no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "cda:i:o:h", long_options, NULL);

        if (c == -1)
            break;

        switch(c)
        {
            case 'c':
                //Compress
                compress = 1;
                break;
            case 'd':
                //Decompress
                compress = 0;
                break;
            case 'a':
                //Algorithm
                if (strcmp(optarg, "huffman") == 0)
                {
                    algorithm = new Huffman();
                    break;
                }
                if (strcmp(optarg, "lz77") == 0)
                {
                    algorithm = new LZ77();
                    break;
                }
                cout << "Algorithm not supported: " << optarg << "\n";
                return 1;
            case 'i':
                //Input file
                file_in = optarg;
                break;
            case 'o':
                //Output file
                file_out = optarg;
                break;
            case 'h':
                //Help
                cout << "\n"
                     << "Usage: compressor "
                     << "(-c | --compress | -d | --decompress) "
                     << "(-a <algorithm> | --algorithm <algorithm>) "
                     << "(-i <file> | --input <file>) "
                     << "(-o <file> | --output <file>)" << "\n"
                     << "\n"
                     << "-c, --compress                            Compress file" << "\n"
                     << "-d, --decompress                          Decompress file" << "\n"
                     << "-a <algorithm>, --algorithm <algorithm>   Algorithm to use" << "\n"
                     << "-i <file>, --input <file>                 Input file" << "\n"
                     << "-o <file>, --output <file>                Output file" << "\n"
                     << "\n"
                     << "Supported algorithms:" << "\n"
                     << "huffman  Canonical Huffman" << "\n"
                     << "lz77     Lempel-Ziv 77" << "\n"
                     << "\n";
                return 0;
            case '?':
                //Unknown argument
                return 1;
            default:
                abort();
        }
    }

    if (compress == -1)
    {
        //Missing operation
        cout << "Missing argument: (-c | --compress | -d | --decompress)" << "\n";
        return 1;
    }

    if (!algorithm)
    {
        //Missing algorithm
        cout << "Missing argument: (-a <algorithm> | --algorithm <algorithm>)" << "\n";
        return 1;
    }

    if (file_in.empty())
    {
        //Missing input file
        cout << "Missing argument: (-i <file> | --input <file>)" << "\n";
        return 1;
    }

    if (file_out.empty())
    {
        //Missing output file
        cout << "Missing argument: (-o <file> | --output <file>)" << "\n";
        return 1;
    }

    if (compress)
        algorithm->compress_file(file_in, file_out);
    else
        algorithm->decompress_file(file_in, file_out);

    delete algorithm;

    //Exit program
    return 0;
}
