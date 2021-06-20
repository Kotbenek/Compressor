#include "Node.h"

#include <cstddef>

Node::Node(int id, bool is_leaf_node)
{
	this->occurences = 0;
	this->id = id;
	this->is_leaf_node = is_leaf_node;
	this->left = NULL;
	this->right = NULL;
	this->parent = NULL;
}

Node::~Node()
{
	if (this->left) delete this->left;
	if (this->right) delete this->right;
}
