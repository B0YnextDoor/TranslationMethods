#include <fstream>
#include <unordered_map>
#include <regex>
#include "Tree.h"

class Parser
{
private:
	TreeBuilder *builder = nullptr;
	std::vector<Token> tokens;
	std::vector<Token> comments;
	std::vector<Token> errors;

	std::string trim(const std::string &str);

	bool isConstant(const std::string &str, char type, size_t lineNumber);
	bool isIdentificator(const std::string &str, size_t lineNumber);

	void processError(std::string error, size_t lineNumber);
	void processComment(std::string str, size_t lineNumber, size_t start);
	size_t processTextLexeme(std::string str, size_t lineNumber, size_t start);
	char defineNumberType(char symb);
	size_t processNumberLexeme(std::string str, size_t lineNumber, size_t start);
	size_t processSeparator(std::string str, size_t lineNumber, size_t start);
	size_t processOperator(std::string str, size_t lineNumber, size_t start);
	size_t processKeyword(std::string str, size_t lineNumber, size_t start);
	void printErrors();
	void analyzeLine(std::string line, size_t lineNumber);

public:
	Parser() {}
	void analyzeFile(const std::string &filename);
	~Parser()
	{
		if (builder)
			delete builder;
	}
};