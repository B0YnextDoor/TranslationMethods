#include "Lexer.h"
#include "Parser.h"
#include "Semantic.h"
#include "Translator.h"

int main()
{
	std::string filepath = "input2.txt";

	Lexer lexer = Lexer();
	std::vector<Token> tokens = lexer.analyzeFile(filepath);
	if (tokens.empty())
		return 0;

	Parser parser = Parser(tokens);
	Node *tree = parser.buildTree();
	if (!tree)
		return 0;

	Semantic semantic = Semantic(tree);
	tree = semantic.analize();
	if (!tree)
		return 0;

	Translator translator = Translator();
	translator.translate(semantic.registers, semantic.identifiers);
}