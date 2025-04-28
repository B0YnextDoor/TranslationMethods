#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <iomanip>

enum TokenType
{
	IDENTIFIER,
	CONSTANT,
	OPERATOR,
	INSTRUCTION,
	KEYWORD,
	SEPARATOR,
	COMMENT,
	ERROR
};

struct Token
{
	size_t id;
	TokenType type;
	std::string value;
	std::string description = "";
};

class Lexer
{
private:
	size_t counter = 0;
	std::unordered_map<std::string, Token> identifierLexems;
	std::unordered_map<std::string, Token> keywordLexems;
	std::unordered_map<std::string, Token> operatorLexems;
	std::unordered_map<std::string, Token> separatorLexems;
	std::unordered_map<std::string, Token> constantLexems;
	std::unordered_map<std::string, Token> comments;
	std::unordered_map<std::string, Token> errors;

	std::string trim(const std::string &str);

	bool isConstant(const std::string &str, char type, size_t lineNumber);
	bool isIdentificator(const std::string &str, size_t lineNumber);
	void addSeparator(std::string sep);

	void processError(std::string error);
	void processComment(std::string str, size_t lineNumber, size_t start);
	size_t processTextLexeme(std::string str, size_t lineNumber, size_t start);
	char defineNumberType(char symb);
	size_t processNumberLexeme(std::string str, size_t lineNumber, size_t start);
	size_t processSeparator(std::string str, size_t lineNumber, size_t start);
	size_t processOperator(std::string str, size_t lineNumber, size_t start);
	size_t processKeyword(std::string str, size_t lineNumber, size_t start);
	std::vector<Token> mapToVector(std::unordered_map<std::string, Token> map);
	void printVector(std::string name, std::vector<Token> values);
	void printTables();
	void analyzeLine(std::string line, size_t lineNumber);

public:
	void analyzeFile(const std::string &filename);
};