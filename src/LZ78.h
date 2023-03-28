#pragma once

#include "CompressionAlgorithm.h"
#include "LZ78_Node.h"
#include "LZ78_Tuple.h"

#include <cstdint>
#include <string>

class LZ78 : public CompressionAlgorithm
{
public:
    /*
    File structure:
    [size of dictionary index in bits] (8-bit value)
    Compressed data consists of tuples:
    [dictionary index][next token]
    If the last tuple is [dictionary index][EOF], it is trimmed
    to [dictionary index] - a denormalized tuple
    */
    void compress_file(std::string file_in, std::string file_out);
    void decompress_file(std::string file_in, std::string file_out);

    LZ78();
    ~LZ78();

private:
    char* buffer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t dictionary_index;

    const uint32_t BUFFER_SIZE = 65536;

    LZ78_Tuple* build_tuple(LZ78_Node* current_node, bool *denormalized);
    int32_t read_input_file(std::ifstream &fs_in, uint32_t length);
    char read_buffer();
    uint32_t buffer_bytes_available();

    uint32_t find_highest_used_tuple_id(std::vector<LZ78_Tuple*> output_data);

    LZ78_Node* find_dictionary_entry(uint32_t id, LZ78_Node* dictionary, std::vector<uint8_t>* path);

    static void write_tuple_8bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);
    static void write_tuple_16bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);
    static void write_tuple_24bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);
    static void write_tuple_32bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);

    static void write_8bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);
    static void write_16bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);
    static void write_24bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);
    static void write_32bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple);

    LZ78_Tuple* read_tuple(uint8_t dictionary_index_size, bool* denormalized);
    LZ78_Tuple* read_tuple_8bit_id(bool* denormalized);
    LZ78_Tuple* read_tuple_16bit_id(bool* denormalized);
    LZ78_Tuple* read_tuple_24bit_id(bool* denormalized);
    LZ78_Tuple* read_tuple_32bit_id(bool* denormalized);
};
