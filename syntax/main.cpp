#include "Parser.h"

int main()
{
	std::string filepath = "input3.txt";

	Parser parser = Parser();
	parser.analyzeFile(filepath);
}