#include "Types.h"
#include "Constants.h"

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
private:
	NodeType type;
	std::string name;
	std::string value;
	size_t line;
	std::vector<Node *> children;

public:
	Node() {}
	Node(NodeType type, std::string name,
		 std::string value = "", size_t line = 0) : type(type), name(name), value(value), line(line) {}

	NodeType Type();
	size_t Line();
	std::string Value();
	void Value(std::string value);
	std::vector<Node *> Children();
	bool hasChildren();
	void addChild(Node *node);
	void print(size_t level = 0);
};

class TreeBuilder
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
	TreeBuilder(std::vector<Token> &tokens);
	void buildTree();
	~TreeBuilder()
	{
		if (root)
			delete root;
	}
};
