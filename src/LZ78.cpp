#include "LZ78.h"

#include <fstream>
#include <iostream>

void LZ78::compress_file(std::string file_in, std::string file_out)
{
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    //Create dictionary root entry (empty string)
    LZ78_Node* dictionary = new LZ78_Node(0, 0);

    std::vector<LZ78_Tuple*> output_data;
    bool last_tuple_denormalized = false;
    bool input_file_end_reached = false;

    //Read the first chunk of file
    if (read_input_file(fs_in, BUFFER_SIZE - 1)) input_file_end_reached = true;

    while (buffer_bytes_available() > 0)
    {
        LZ78_Tuple* tuple = build_tuple(dictionary, &last_tuple_denormalized);

        //Read next data chunk
        if (!input_file_end_reached)
            if (read_input_file(fs_in, BUFFER_SIZE - buffer_bytes_available() - 1))
                input_file_end_reached = true;

        output_data.push_back(tuple);
    }

    //Find out how many bits are needed for tuple id
    uint32_t highest_used_tuple_id = find_highest_used_tuple_id(output_data);
    void (*write_tuple)(std::ofstream &fs_out, LZ78_Tuple* tuple);

    if (highest_used_tuple_id < 256)
    {
        fs_out << (uint8_t)8;
        write_tuple = &write_tuple_8bit_id;
    }
    else if (highest_used_tuple_id < 65536)
    {
        fs_out << (uint8_t)16;
        write_tuple = &write_tuple_16bit_id;
    }
    else if (highest_used_tuple_id < 16777216)
    {
        fs_out << (uint8_t)24;
        write_tuple = &write_tuple_24bit_id;
    }
    else
    {
        fs_out << (uint8_t)32;
        write_tuple = &write_tuple_32bit_id;
    }

    for (uint64_t i = 0; i < output_data.size() - 1; i++)
        write_tuple(fs_out, output_data[i]);

    if (last_tuple_denormalized)
    {
        if (highest_used_tuple_id < 256)
            write_8bit_id(fs_out, output_data[output_data.size() - 1]);
        else if (highest_used_tuple_id < 65536)
            write_16bit_id(fs_out, output_data[output_data.size() - 1]);
        else if (highest_used_tuple_id < 16777216)
            write_24bit_id(fs_out, output_data[output_data.size() - 1]);
        else
            write_32bit_id(fs_out, output_data[output_data.size() - 1]);
    }
    else
        write_tuple(fs_out, output_data[output_data.size() - 1]);

    //Cleanup
    delete dictionary;
    for (uint64_t i = 0; i < output_data.size(); i++)
        delete output_data[i];
    std::vector<LZ78_Tuple*>().swap(output_data);
}

LZ78_Tuple* LZ78::build_tuple(LZ78_Node* current_node, bool *denormalized)
{
    //Denormalized tuple
    if (buffer_bytes_available() == 0)
    {
        *denormalized = true;
        return new LZ78_Tuple(current_node->id, 0);
    }

    uint8_t buf = read_buffer();

    LZ78_Node* found_node = LZ78_Node::find_node_with_token(current_node, buf);

    if (found_node == NULL)
    {
        //Create node
        LZ78_Node* node = new LZ78_Node(++dictionary_index, buf);
        current_node->leaves.push_back(node);
        return new LZ78_Tuple(current_node->id, node->token);
    }
    else
    {
        //Found node, try to dig deeper
        return build_tuple(found_node, denormalized);
    }
}

void LZ78::decompress_file(std::string file_in, std::string file_out)
{
    std::ifstream fs_in(file_in, std::ifstream::binary);
    std::ofstream fs_out(file_out, std::ofstream::binary);

    //Create dictionary root entry (empty string)
    LZ78_Node* dictionary = new LZ78_Node(0, 0);

    bool last_tuple_denormalized = false;
    bool input_file_end_reached = false;

    uint8_t dictionary_index_size;
    fs_in >> dictionary_index_size;

    //Read the first chunk of file
    if (read_input_file(fs_in, BUFFER_SIZE - 1)) input_file_end_reached = true;

    while (buffer_bytes_available() > 0)
    {
        LZ78_Tuple* tuple = read_tuple(dictionary_index_size, &last_tuple_denormalized);

        //Traverse dictionary depth-first, saving the path; append new node to found entry
        std::vector<uint8_t> path;
        find_dictionary_entry(tuple->id, dictionary, &path)->leaves.push_back(new LZ78_Node(++dictionary_index, tuple->token));

        //Append path and token to output
        for (uint64_t i = 0; i < path.size(); i++)
            fs_out << path[i];
        if (!last_tuple_denormalized)
            fs_out << tuple->token;

        //Read next data chunk
        if (!input_file_end_reached)
            if (read_input_file(fs_in, BUFFER_SIZE - buffer_bytes_available() - 1))
                input_file_end_reached = true;
    }
}

int32_t LZ78::read_input_file(std::ifstream &fs_in, uint32_t length)
{
    //Case: reading to the middle of the buffer
    if (write_pointer + length < BUFFER_SIZE)
    {
        fs_in.read(buffer + write_pointer, length);

        int32_t bytes_read = fs_in.gcount();
        write_pointer += bytes_read;

        return length - bytes_read;
    }

    //Case: reading to the end of the buffer
    if (write_pointer + length == BUFFER_SIZE)
    {
        fs_in.read(buffer + write_pointer, length);

        int32_t bytes_read = fs_in.gcount();
        write_pointer += bytes_read;
        write_pointer -= (write_pointer >= BUFFER_SIZE) * BUFFER_SIZE;

        return length - bytes_read;
    }

    //Case: reading with crossing the end of the buffer
    {
        fs_in.read(buffer + write_pointer, BUFFER_SIZE - write_pointer);

        int32_t bytes_read = fs_in.gcount();
        write_pointer += bytes_read;
        write_pointer -= (write_pointer >= BUFFER_SIZE) * BUFFER_SIZE;

        if (write_pointer != 0) return bytes_read;

        fs_in.read(buffer, length - bytes_read);

        int32_t bytes_read_2 = fs_in.gcount();
        write_pointer += bytes_read_2;
        write_pointer -= (write_pointer >= BUFFER_SIZE) * BUFFER_SIZE;

        return length - (bytes_read + bytes_read_2);
    }
}

char LZ78::read_buffer()
{
    char buf = buffer[read_pointer++];
    read_pointer %= BUFFER_SIZE;
    return buf;
}

uint32_t LZ78::buffer_bytes_available()
{
    //Safe if (BUFFER_SIZE + write_pointer - read_pointer) < 2 * BUFFER_SIZE
    return (BUFFER_SIZE + write_pointer - read_pointer) - ((BUFFER_SIZE + write_pointer - read_pointer) >= BUFFER_SIZE) * BUFFER_SIZE;
}

uint32_t LZ78::find_highest_used_tuple_id(std::vector<LZ78_Tuple*> output_data)
{
    uint32_t highest = 0;

    for (uint64_t i = 0; i < output_data.size(); i++)
    {
        if (output_data[i]->id > highest)
            highest = output_data[i]->id;
    }

    return highest;
}

LZ78_Node* LZ78::find_dictionary_entry(uint32_t id, LZ78_Node* dictionary, std::vector<uint8_t>* path)
{
    //Traverse dictionary depth-first, saving the path; return when id is found
    if (dictionary->id == id)
    {
        if (dictionary->id != 0)
            path->push_back(dictionary->token);
        return dictionary;
    }

    for (uint16_t i = 0; i < dictionary->leaves.size(); i++)
    {
        if (dictionary->id != 0)
            path->push_back(dictionary->token);
        LZ78_Node* entry = find_dictionary_entry(id, dictionary->leaves[i], path);
        if (entry != NULL)
            return entry;
        else if (dictionary->id != 0)
            path->pop_back();
    }

    return NULL;
}

void LZ78::write_tuple_8bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    write_8bit_id(fs_out, tuple);
    fs_out << tuple->token;
}

void LZ78::write_tuple_16bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    write_16bit_id(fs_out, tuple);
    fs_out << tuple->token;
}

void LZ78::write_tuple_24bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    write_24bit_id(fs_out, tuple);
    fs_out << tuple->token;
}

void LZ78::write_tuple_32bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    write_32bit_id(fs_out, tuple);
    fs_out << tuple->token;
}

void LZ78::write_8bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    fs_out << (uint8_t)tuple->id;
}

void LZ78::write_16bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    fs_out << (uint8_t)(tuple->id >> 8);
    fs_out << (uint8_t)(tuple->id >> 0);
}

void LZ78::write_24bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    fs_out << (uint8_t)(tuple->id >> 16);
    fs_out << (uint8_t)(tuple->id >> 8);
    fs_out << (uint8_t)(tuple->id >> 0);
}

void LZ78::write_32bit_id(std::ofstream &fs_out, LZ78_Tuple* tuple)
{
    fs_out << (uint8_t)(tuple->id >> 24);
    fs_out << (uint8_t)(tuple->id >> 16);
    fs_out << (uint8_t)(tuple->id >> 8);
    fs_out << (uint8_t)(tuple->id >> 0);
}

LZ78_Tuple* LZ78::read_tuple(uint8_t dictionary_index_size, bool* denormalized)
{
    switch (dictionary_index_size)
    {
        case 8:
            return read_tuple_8bit_id(denormalized);
        case 16:
            return read_tuple_16bit_id(denormalized);
        case 24:
            return read_tuple_24bit_id(denormalized);
        case 32:
            return read_tuple_32bit_id(denormalized);
        default:
            std::cout << "Incorrect dictionary index size." << "\n";
            return NULL;
    }
}

LZ78_Tuple* LZ78::read_tuple_8bit_id(bool* denormalized)
{
    LZ78_Tuple* tuple = new LZ78_Tuple();

    tuple->id = read_buffer();

    if (buffer_bytes_available() > 0)
        tuple->token = read_buffer();
    else
        *denormalized = true;

    return tuple;
}

LZ78_Tuple* LZ78::read_tuple_16bit_id(bool* denormalized)
{
    LZ78_Tuple* tuple = new LZ78_Tuple();
    uint8_t buf;

    buf = read_buffer();
    tuple->id = (uint32_t)buf << 8;
    buf = read_buffer();
    tuple->id |= buf;

    if (buffer_bytes_available() > 0)
        tuple->token = read_buffer();
    else
        *denormalized = true;

    return tuple;
}

LZ78_Tuple* LZ78::read_tuple_24bit_id(bool* denormalized)
{
    LZ78_Tuple* tuple = new LZ78_Tuple();
    uint8_t buf;

    buf = read_buffer();
    tuple->id = (uint32_t)buf << 16;
    buf = read_buffer();
    tuple->id |= (uint32_t)buf << 8;
    buf = read_buffer();
    tuple->id |= buf;

    if (buffer_bytes_available() > 0)
        tuple->token = read_buffer();
    else
        *denormalized = true;

    return tuple;
}

LZ78_Tuple* LZ78::read_tuple_32bit_id(bool* denormalized)
{
    LZ78_Tuple* tuple = new LZ78_Tuple();
    uint8_t buf;

    buf = read_buffer();
    tuple->id = (uint32_t)buf << 24;
    buf = read_buffer();
    tuple->id |= (uint32_t)buf << 16;
    buf = read_buffer();
    tuple->id |= (uint32_t)buf << 8;
    buf = read_buffer();
    tuple->id |= buf;

    if (buffer_bytes_available() > 0)
        tuple->token = read_buffer();
    else
        *denormalized = true;

    return tuple;
}

LZ78::LZ78()
{
    buffer = new char[BUFFER_SIZE];
    write_pointer = 0;
    read_pointer = 0;
    dictionary_index = 0;
}

LZ78::~LZ78()
{
    delete[] buffer;
}