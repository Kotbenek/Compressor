#include "Huffman.h"

#include "Node.h"
#include "Sort.h"
#include "HuffmanCodebook.h"

#include <string>
#include <iostream>
#include <queue>
#include <bitset>
#include <fstream>

using namespace std;

void Huffman::count_occurences(string file)
{
    //Count byte occurences in a file
    ifstream fs(file, ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];
    
    while (fs.good())
    {
        fs.read(buffer, BUFFER_SIZE);
        int32_t bytes_read = fs.gcount();
        
        for (int32_t i = 0; i < bytes_read; i++)
        {
            nodes[(uint8_t)buffer[i]]->occurences++;
        }
    }
    
    delete[] buffer;
    fs.close();
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
        if (!nodes[i]->occurences) break;
        queue.push(nodes[i]);
    }
    leaf_nodes = queue.size();
    
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
            //Increment codeword length
            codeword_length++;
            
            //If node is right leaf of parent, set bit at this position
            if (!n->parent) break;
            if (n->parent->right == n) codeword |= (1 << position);
            
            //Move position
            position--;
            
            //Move to parent
            n = n->parent;
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
}

void Huffman::read_canonical_codebook(string file)
{
    //Prepare input stream
    ifstream fs(file, ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];
    
    if (!fs.good())
    {
        cout << "Input file error";
        return;
    }
    
    //Read file header
    fs.read(buffer, 8);
    compressed_data_starts_at += 8;
        
    uint16_t dictionary_size = ((uint16_t)buffer[0] & 0xFF) + 1;
    
    original_file_size = 0;
    original_file_size |= (((uint64_t)buffer[1] & 0xFF) << 48);
    original_file_size |= (((uint64_t)buffer[2] & 0xFF) << 40);
    original_file_size |= (((uint64_t)buffer[3] & 0xFF) << 32);
    original_file_size |= (((uint64_t)buffer[4] & 0xFF) << 24);
    original_file_size |= (((uint64_t)buffer[5] & 0xFF) << 16);
    original_file_size |= (((uint64_t)buffer[6] & 0xFF) <<  8);
    original_file_size |= (((uint64_t)buffer[7] & 0xFF) <<  0);
    
    //Recreate codebook
    codebook = new HuffmanCodebook(dictionary_size);
    
    //Read number of symbols with codeword length of 1...n
    uint16_t dictionary_size_counter = 0;
    uint16_t* number_of_symbols_with_codeword_length = new uint16_t[32];
    for (uint8_t i = 0; i < 32; i++) number_of_symbols_with_codeword_length[i] = 0;
    for (uint8_t i = 0; i < 32; i++)
    {
        fs.read(buffer, 1);
        compressed_data_starts_at++;
        number_of_symbols_with_codeword_length[i] = (buffer[0] & 0xFF);
        dictionary_size_counter += (buffer[0] & 0xFF);
        if (dictionary_size_counter == dictionary_size)
        {
            dictionary_size_counter = i;
            break;
        }
    }
    if (!dictionary_size_counter) number_of_symbols_with_codeword_length[7] = 256;
    
    //Set codeword lengths
    for (uint16_t i = 0; i < dictionary_size; i++)
    {
        for (uint8_t j = 0; j < 32; j++)
        {
            if (number_of_symbols_with_codeword_length[j])
            {
                number_of_symbols_with_codeword_length[j]--;
                codebook->codes_length[i] = j + 1;
                break;
            }
        }
    }
    
    //Set the first symbol to 0
    codebook->codes[0] = 0;
    for (uint32_t i = 1; i < codebook->size; i++)
    {
        //For each symbol starting from the second one, assign next binary number
        //If the codeword is longer than the previous one, append zeros (left shift)
        codebook->codes[i] = (codebook->codes[i - 1] + 1) << (codebook->codes_length[i] - codebook->codes_length[i - 1]);
    }
    
    //Read codeword values
    fs.read(buffer, dictionary_size);
    compressed_data_starts_at += dictionary_size;
    for (uint16_t i = 0; i < dictionary_size; i++)
    {        
        codebook->codes_value[i] = buffer[i];
    }
    
    delete[] buffer;
    delete[] number_of_symbols_with_codeword_length;
    fs.close();
}

void Huffman::compress(string file_in, string file_out)
{
    //Prepare input and output streams
    ifstream fs_in(file_in, ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];
    ofstream fs_out(file_out, ofstream::binary);
    
    //Prepare file header
    uint8_t dictionary_size = codebook->size & 0xFF;
    
    original_file_size = 0;
    for (uint16_t i = 0; i < 256; i++)
    {
        if (!nodes[i]->occurences) break;
        
        original_file_size += nodes[i]->occurences;
    }
    
    uint8_t* number_of_symbols_with_codeword_length = new uint8_t[32];
    for (uint8_t i = 0; i < 32; i++) number_of_symbols_with_codeword_length[i] = 0;
    for (uint16_t i = 0; i < codebook->size; i++)
    {
        number_of_symbols_with_codeword_length[codebook->codes_length[i] - 1]++;
    }
    
    uint8_t number_of_symbols_with_codeword_length_highest = 31;
    for (int8_t i = 31; i > 0; i--)
    {
        if (number_of_symbols_with_codeword_length[i])
        {
            number_of_symbols_with_codeword_length_highest = i;
            break;
        }
    }
    
    //Write file header
    fs_out << (uint8_t)(dictionary_size - 1);
    
    fs_out << (uint8_t)((original_file_size >> 48) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 40) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 32) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 24) & 0xFF);
    fs_out << (uint8_t)((original_file_size >> 16) & 0xFF);
    fs_out << (uint8_t)((original_file_size >>  8) & 0xFF);
    fs_out << (uint8_t)((original_file_size >>  0) & 0xFF);
        
    for (uint8_t i = 0; i <= number_of_symbols_with_codeword_length_highest; i++)
    {
        fs_out << (number_of_symbols_with_codeword_length[i]);
    }
    
    for (uint16_t i = 0; i < codebook->size; i++)
    {
        fs_out << codebook->codes_value[i];
    }
    
    //Compress the input file
    uint8_t data_out = 0;
    int8_t data_out_position = 7;
    
    while (fs_in.good())
    {
        fs_in.read(buffer, BUFFER_SIZE);
        int32_t bytes_read = fs_in.gcount();
        
        for (int32_t i = 0; i < bytes_read; i++)
        {
            //For each byte in buffer, run through codebook
            for (uint16_t j = 0; j < codebook->size; j++)
            {
                //If current byte from buffer matches this codebook record, write codeword to the output file
                if (codebook->codes_value[j] == (uint8_t)buffer[i])
                {
                    for (int8_t k = codebook->codes_length[j] - 1; k >= 0; k--)
                    {
                        data_out |= (((codebook->codes[j] >> (k)) & 0x01) << (data_out_position--));
                        if (data_out_position < 0)
                        {
                            fs_out << data_out;
                            data_out = 0;
                            data_out_position = 7;                            
                        }
                    }
                    break;
                }
            }            
        }
    }
    
    //If there is any leftover data, write it to the output file
    if (data_out_position != 7)
    {
        fs_out << data_out;
    }
    
    delete[] buffer;
    delete[] number_of_symbols_with_codeword_length;
    fs_in.close();
    fs_out.close();
}

void Huffman::decompress(string file_in, string file_out)
{
    //Prepare input and output streams
    ifstream fs_in(file_in, ifstream::binary);
    char* buffer = new char[BUFFER_SIZE];
    ofstream fs_out(file_out, ofstream::binary);
    
    //Skip to data
    fs_in.read(buffer, compressed_data_starts_at);
    
    //Decompress the input file
    uint32_t codeword = 0;
    uint8_t codeword_length = 0;
    uint64_t bytes_decompressed = 0;
    while(fs_in.good())
    {
        fs_in.read(buffer, BUFFER_SIZE);
        int32_t bytes_read = fs_in.gcount();
        
        for (int32_t i = 0; i < bytes_read; i++)
        {
            for (int8_t j = 7; j >= 0; j--)
            {
                codeword <<= 1;
                codeword |= (((uint32_t)buffer[i] >> j) & 0x01);
                codeword_length++;
                for (uint16_t k = 0; k < codebook->size; k++)
                {
                    if (codebook->codes[k] == codeword && codebook->codes_length[k] == codeword_length)
                    {
                        fs_out << codebook->codes_value[k];
                        codeword = 0;
                        codeword_length = 0;
                        bytes_decompressed++;
                        if (bytes_decompressed == original_file_size)
                        {
                            k = codebook->size;
                            j = -1;
                            i = bytes_read;
                            fs_in.close();
                        }
                    }
                }
            }
        }
    }
    
    delete[] buffer;
    fs_out.close();
}

void Huffman::compress_file(string file_in, string file_out)
{
    count_occurences(file_in);
    sort_nodes();
    construct_tree();
    create_codebook();
    create_canonical_codebook();
    compress(file_in, file_out);
}

void Huffman::decompress_file(string file_in, string file_out)
{
    read_canonical_codebook(file_in);
    decompress(file_in, file_out);
}

Huffman::Huffman()
{
    this->nodes = new Node*[256];
    for (uint16_t i = 0; i < 256; i++)
    {
        this->nodes[i] = new Node(i, true);
    }
    this->leaf_nodes = 0;
    this->compressed_data_starts_at = 0;
    this->original_file_size = 0;
    this->tree = NULL;
    this->codebook = NULL;
}

Huffman::~Huffman()
{
    if (tree) tree->~Node();
    delete[] nodes;
    if (codebook) delete codebook;
}
