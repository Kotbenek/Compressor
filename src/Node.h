#pragma once

#include <cstdint>

class Node
{
public:
    int32_t id;
    uint64_t occurrences;
    bool is_leaf_node;
    Node* left;
    Node* right;
    Node* parent;

    Node(int32_t id, bool is_leaf_node);
    ~Node();
};
