#include "Lexer.h"
#include "Parser.h"
#include "Semantic.h"

int main()
{
	std::string filepath = "input3.txt";
	Lexer lexer = Lexer();
	std::vector<Token> tokens = lexer.analyzeFile(filepath);
	if (tokens.empty())
		return 0;
	Parser parser = Parser(tokens);
	Node *tree = parser.buildTree();
	if (!tree)
		return 0;
	Semantic semantic = Semantic(tree);
	semantic.analize();
}