#include "Token.h"
#include "Node.h"
#include "Constants.h"

class Parser
{
private:
	Node *root = nullptr;
	bool isError = false;
	size_t index = 0;
	size_t len = 0;
	std::vector<Token> tokens;
	std::vector<Token> errors;

	void processError(std::string message, size_t line);
	Node *processLiteral();
	Node *processIdentifier();
	Node *processKeyword();
	int precedence(const Token &token);
	std::vector<Token> toReverseNotation(std::vector<Token> &tokens);
	Node *createExpressionNode(std::vector<Token> &tokens, size_t line, size_t start);
	Node *processExpression();
	bool isSection(Token &token, Token &tokenNext);
	Node *processInitialization();
	Node *processInstruction();
	Node *checkChild(Node *parent, Node *node, bool isInit);
	Node *checkDuplicates(Node *node);
	void panicMode();
	Node *processSection();
	void printErrors();

public:
	Parser(std::vector<Token> &tokens);
	Node *buildTree();
	~Parser()
	{
		if (root)
			delete root;
	}
};
