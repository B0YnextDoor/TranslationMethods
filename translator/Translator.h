#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "Node.h"
#include "DataType.h"
#include <cmath>

class Translator
{
private:
	std::map<std::string, DataType *> variables;
	std::map<std::string, Node *> functions;
	std::stack<DataType *> stack;
	Node *func = nullptr;
	Node *callFunc = nullptr;
	bool isLoop = false;
	long long nodeIdx = -1;
	int level = 1;
	std::string cmpRes = "";

	void printTab();
	void Printf();
	char defineNumberType(std::string);
	void setLiteral(DataType *, Node *);
	size_t fillVariables(std::vector<Node *>, bool);
	DataType *shiftVariable(std::string, DataType *, size_t);
	DataType *processExpression(Node *);
	void processAssigment(Node *);
	int processInteruption(Node *);
	void processStack(Node *, bool);
	std::pair<DataType *, DataType *> getArgs(std::vector<Node *>);
	std::string toUpper(std::string);
	void processArithmetic(Node *, std::string);
	void processLogic(Node *, std::string);
	void processCmp(Node *);
	void processLoop();
	int processCondition(Node *, std::string);
	int processCall(Node *);
	int processInstruction(Node *);
	int processFunction(Node *, bool);

public:
	void translate(std::vector<Node *>, std::vector<Node *>);
};

#endif