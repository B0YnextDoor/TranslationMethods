#include <limits>
#include <sstream>
#include <cmath>
#include "Token.h"
#include "Node.h"
#include "Constants.h"

class Semantic
{
private:
	Node *root = nullptr;
	std::vector<Node *> identifiers;
	std::vector<Token> errors;

	void processError(std::string message, size_t line);
	size_t processTextLiteral(int typeSize, Node *node);
	char defineNumberType(std::string num);
	std::string transformNumber(std::string num, char type);
	std::string checkFitFloat(std::string num, int typeSize, size_t line, char type);
	std::string checkFitInt(std::string num, int typeSize, size_t line, char type, bool isSigned);
	size_t processNumberLiteral(int typeSize, Node *node, bool isFloatingPoint);
	std::vector<std::pair<std::string, int>> getTypes(bool isFloatingPoint);
	void defineLiteralType(Node *node);
	void processDefine(Node *node);
	void processInitialization(Node *node);
	Node *findNode(std::string value, bool isRoot);
	void processRegister(Node *node, Node *prevNode, std::string type);
	void processIdentifier(Node *node, Node *prevNode, std::string type);
	void processLiteral(Node *node, Node *prevNode, std::string type);
	void processChildren(std::vector<Node *> children, std::string nodeValue);
	bool checkExpression(Node *node);
	void processExpression(Node *node, Node *prevNode, std::string type);
	void processStream(Node *node);
	void processInstruction(Node *node);
	void processNode(Node *node);
	void processSection(Node *node);
	void printErrors();

public:
	Semantic(Node *tree);
	void analize();
};