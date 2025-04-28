#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

enum NodeType
{
	SECTION,
	INSTRUCTION,
	INITIALIZATION,
	EXPRESSION,
	NONE,
};

class Node
{
protected:
	NodeType type;
	std::string name;
	std::string value;
	std::string valueType;
	size_t line;
	std::vector<Node *> children;

public:
	Node();
	Node(NodeType type, std::string name,
		 std::string value = "", size_t line = 0);
	NodeType Type();
	size_t Line();
	std::string Value();
	void Value(std::string value);
	std::string ValueType();
	void ValueType(std::string type);
	std::string Name();
	std::vector<Node *> Children();
	bool hasChildren();
	void addChild(Node *node);
	void print(size_t level = 0);
};

#endif