#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

enum TokenType
{
	IDENTIFIER,
	CONSTANT,
	OPERATOR,
	KEYWORD,
	SEPARATOR,
	COMMENT,
	ERROR,
};

struct Token
{
	size_t id;
	TokenType type;
	std::string value;
	std::string description = "";
	size_t line = 0;

	Token(size_t id, TokenType type, std::string value,
		  std::string description = "", size_t line = 0) : id(id), type(type), value(value), line(line),
														   description(description) {}
};