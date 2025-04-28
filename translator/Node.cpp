#include "Node.h"

Node::Node() {}

Node::Node(NodeType type, std::string name, std::string value, size_t line) : type(type), name(name), value(value), line(line), valueType("") {}

NodeType Node::Type()
{
	return type;
}

size_t Node::Line()
{
	return line;
}

std::string Node::Value()
{
	return value;
}

void Node::Value(std::string value)
{
	this->value = value;
}

std::string Node::ValueType()
{
	return valueType;
}

void Node::ValueType(std::string type)
{
	this->valueType = type;
}

std::string Node::Name()
{
	return name;
}

std::vector<Node *> Node::Children()
{
	return children;
}

bool Node::hasChildren()
{
	return !(children.empty());
}

void Node::addChild(Node *node)
{
	children.push_back(node);
}

void Node::print(size_t level)
{
	for (int i = 0; i < level; ++i)
		std::cout << "  ";
	std::cout << name << ": " << value;
	if (!valueType.empty())
		std::cout << " - " << valueType;
	std::cout << "\n";
	for (auto child : children)
	{
		child->print(level + 1);
	}
}
