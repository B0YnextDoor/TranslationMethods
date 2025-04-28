#include "Lexer.h"

int main()
{
	std::string filepath = "input2.txt";

	Lexer lexer = Lexer();
	lexer.analyzeFile(filepath);
}