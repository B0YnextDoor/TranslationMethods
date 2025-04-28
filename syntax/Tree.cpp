#include "Tree.h"

NodeType Node::Type()
{
	return type;
}

size_t Node::Line()
{
	return line;
}

std::string Node::Value()
{
	return value;
}

void Node::Value(std::string value)
{
	this->value = value;
}

std::vector<Node *> Node::Children()
{
	return children;
}

bool Node::hasChildren()
{
	return !(this->children.empty());
}

void Node::addChild(Node *node)
{
	children.push_back(node);
}

void Node::print(size_t level)
{
	for (int i = 0; i < level; ++i)
		std::cout << "  ";
	std::cout << name << ": " << value << "\n";
	for (auto child : children)
	{
		child->print(level + 1);
	}
}

void TreeBuilder::processError(std::string message, size_t line)
{
	message += " in line " + std::to_string(line) + "!";
	isError = true;
	errors.push_back(Token(errors.size() + 1, ERROR, message, "", line));
}

Node *TreeBuilder::processLiteral()
{
	Token token = tokens[index];
	return new Node(NONE, "LITERAL_NODE", token.value, token.line);
}

Node *TreeBuilder::processIdentifier()
{
	Token token = tokens[index];
	return new Node(NONE, "IDENTIFIER_NODE", token.value, token.line);
}

Node *TreeBuilder::processKeyword()
{
	Token token = tokens[index];
	return new Node(NONE, "KEYWORD_NODE", token.value, token.line);
}

int TreeBuilder::precedence(const Token &token)
{
	std::string op = token.value;
	if (op == "*" || op == "/" || op == "//")
		return 5;
	if (op == "+" || op == "-")
		return 4;
	if (op == ">>" || op == "<<")
		return 3;
	if (op == "&")
		return 2;
	if (op == "^")
		return 1;
	return 0;
}

std::vector<Token> TreeBuilder::toReverseNotation(std::vector<Token> &tokens)
{
	std::vector<Token> res;
	std::stack<Token> ops;
	std::string LBRACE = std::string(1, LBRACES[0]), RBRACE = std::string(1, RBRACES[0]);

	for (const auto &token : tokens)
	{
		if (token.type == IDENTIFIER || token.type == CONSTANT || token.type == KEYWORD)
			res.push_back(token);
		else if (token.value == LBRACE)
			ops.push(token);
		else if (token.value == RBRACE)
		{
			while (!ops.empty() && ops.top().value != LBRACE)
			{
				res.push_back(ops.top());
				ops.pop();
			}
			if (!ops.empty() && ops.top().value == LBRACE)
				ops.pop();
		}
		else if (token.type == OPERATOR)
		{
			int prec = precedence(token);
			while (!ops.empty() && precedence(ops.top()) >= prec)
			{
				res.push_back(ops.top());
				ops.pop();
			}
			ops.push(token);
		}
	}

	while (!ops.empty())
	{
		res.push_back(ops.top());
		ops.pop();
	}
	std::reverse(res.begin(), res.end());
	return res;
}

Node *TreeBuilder::createExpressionNode(std::vector<Token> &tokens, size_t line, size_t start = 0)
{
	size_t n = tokens.size();
	std::vector<Node *> buffer;
	Node *node = new Node(EXPRESSION, "EXPRESSION_NODE", "", line), *tmp = nullptr;
	Token token = tokens[start];
	while (start < n)
	{
		if (token.type == OPERATOR && node->Value().empty())
			node->Value(token.value);
		else if (token.type == OPERATOR && !node->Value().empty())
		{
			if (buffer.size() == 2)
				break;
			buffer.push_back(createExpressionNode(tokens, line, start));
			start += 2;
		}
		else if (token.type == CONSTANT)
		{
			tmp = processLiteral();
			tmp->Value(token.value);
			buffer.push_back(tmp);
		}
		else if (token.type == IDENTIFIER)
		{
			tmp = processIdentifier();
			tmp->Value(token.value);
			buffer.push_back(tmp);
		}
		else if (token.type == KEYWORD)
		{
			tmp = processKeyword();
			tmp->Value(token.value);
			buffer.push_back(tmp);
		}
		if (start + 1 >= n)
			break;
		token = tokens[++start];
	}
	for (int i = 1; i >= 0; --i)
	{
		node->addChild(buffer[i]);
	}
	return node;
}

Node *TreeBuilder::processExpression()
{
	Token token = tokens[index++];
	std::string stop = token.value == "[" ? "]" : ")";
	token = tokens[index];
	size_t nodeLine = token.line;
	Token prevToken = token;
	std::vector<Token> buffer;
	bool hasOperator = false;
	while (index < len && token.value != stop)
	{
		if (token.type == IDENTIFIER)
		{
			if (!buffer.empty() && prevToken.type != OPERATOR)
			{
				processError("Invalid expression syntax! OPERATOR expected", token.line);
				return nullptr;
			}
		}
		else if (token.type == CONSTANT)
		{
			if (token.description.find("Text") != std::string::npos)
			{
				processError("Invalid expression syntax! Number LITERAL expected", token.line);
				return nullptr;
			}
			if (!buffer.empty() && prevToken.type != OPERATOR)
			{
				processError("Invalid expression syntax! OPERATOR expected", token.line);
				return nullptr;
			}
		}
		else if (token.type == OPERATOR)
		{
			if (buffer.empty())
			{
				processError("Invalid expression syntax! IDENTIFIER, LITERAL OR REGISTER expected", token.line);
				return nullptr;
			}
			hasOperator = true;
		}
		else if (token.type == KEYWORD)
		{
			if (token.description.find("register") == std::string::npos)
			{
				processError("Invalid expression syntax! REGISTER expected", token.line);
				return nullptr;
			}
			if (!buffer.empty() && prevToken.type != OPERATOR)
			{
				processError("Invalid expression syntax! OPERATOR expected", token.line);
				return nullptr;
			}
		}
		else if (token.type == SEPARATOR)
		{
			std::string LBRACE = std::string(1, LBRACES[0]), RBRACE = std::string(1, RBRACES[0]);
			if ((token.value != LBRACE && token.value != RBRACE) || (token.value == LBRACE && index + 1 == len))
			{
				processError("Invalid expression syntax! Invalid separator", token.line);
				return nullptr;
			}
		}
		else
		{
			processError("Invalid expression syntax", token.line);
			return nullptr;
		}
		buffer.push_back(token);
		if (index + 1 == len)
		{
			++index;
			break;
		}
		prevToken = token;
		token = tokens[++index];
	}
	Node *expression = nullptr;
	if (buffer.empty())
		processError("Invalid expression syntax", token.line);
	else if (hasOperator && buffer.size() < 3)
		processError("Invalid expression syntax! OPERATOR second argument expected", token.line);
	else if (buffer.size() == 1)
		expression = new Node(EXPRESSION, "EXPRESSION_NODE", buffer[0].value, nodeLine);
	else
	{
		auto rpn = toReverseNotation(buffer);
		expression = createExpressionNode(rpn, nodeLine);
	}
	return expression;
}

bool TreeBuilder::isSection(Token &token, Token &tokenNext)
{
	auto checkKeyword = std::find(SECTION_NODE.begin(), SECTION_NODE.end(), token.value);
	return (token.type == KEYWORD && checkKeyword != SECTION_NODE.end()) ||
		   (token.type == IDENTIFIER && tokenNext.type == SEPARATOR);
}

Node *TreeBuilder::processInitialization()
{
	Token token = tokens[index];
	size_t nodeLine = token.line;
	std::vector<Node *> buffer;
	Token prevToken = token;
	std::string nodeValue = "";
	while (index < len)
	{
		if (token.type == IDENTIFIER && buffer.empty())
		{
			buffer.push_back(processIdentifier());
		}
		else if (token.type == KEYWORD && !buffer.empty())
		{
			if (std::find(INITIALIZATION_NODE.begin(), INITIALIZATION_NODE.end(), token.value) ==
				INITIALIZATION_NODE.end())
			{
				processError("Wrong initialization syntax! INITIALIZATION KEYWORD expected", token.line);
				return nullptr;
			}
			if (!nodeValue.empty())
			{
				processError("Wrong initialization syntax! Too many INITIALIZATION KEYWORDS", token.line);
				return nullptr;
			}
			buffer.push_back(processKeyword());
			nodeValue = token.value;
		}
		else if (token.type == CONSTANT && !buffer.empty())
		{
			if (buffer.size() < 3 && prevToken.type != KEYWORD)
			{
				processError("Wrong initialization syntax! INITIALIZATION KEYWORD expected", token.line);
				return nullptr;
			}
			else if (buffer.size() >= 3 && prevToken.value != COMMA)
			{
				processError("Wrong initialization syntax! COMMA expected", token.line);
				return nullptr;
			}
			buffer.push_back(processLiteral());
		}
		else if (token.type == SEPARATOR && index + 1 < len && !buffer.empty())
		{
			if (token.value != COMMA)
			{
				processError("Wrong initialization syntax! COMMA expected", token.line);
				return nullptr;
			}
			else if (prevToken.type != CONSTANT || tokens[index + 1].type != CONSTANT)
			{
				processError("Wrong initialization syntax! LITERAL expected", token.line);
				return nullptr;
			}
		}
		else
		{
			processError("Wrong initialization syntax! IDENTIFIER expected", token.line);
			return nullptr;
		}
		if (index + 1 == len)
		{
			++index;
			break;
		}
		prevToken = tokens[index++];
		token = tokens[index];
		auto check = std::find(INITIALIZATION_NODE.begin(), INITIALIZATION_NODE.end(), token.value);
		if (token.type == IDENTIFIER ||
			(token.type == KEYWORD && check == INITIALIZATION_NODE.end()))
			break;
	}
	if (buffer.empty() || buffer.size() < 3)
	{
		processError("Wrong initialization syntax! Invalid arguments", prevToken.line);
		return nullptr;
	}
	Node *initialization = new Node(INITIALIZATION, "INITIALIZATION_NODE", nodeValue, nodeLine);
	buffer.erase(buffer.begin() + 1);
	for (const auto &val : buffer)
	{
		initialization->addChild(val);
	}
	return initialization;
}

Node *TreeBuilder::processInstruction()
{
	Token token = tokens[index];
	size_t nodeLine = token.line;
	std::vector<Node *> buffer;
	Token prevToken = token;
	while (index < len)
	{
		if (index + 1 < len && (token.type == KEYWORD || token.type == IDENTIFIER) &&
			isSection(token, tokens[index + 1]))
			return nullptr;
		if (token.type == KEYWORD)
		{
			if (std::find(INITIALIZATION_NODE.begin(), INITIALIZATION_NODE.end(), token.value) !=
				INITIALIZATION_NODE.end())
			{
				processError("INITIALIZATION KEYWORD can't be used in code segment. Invalid KEYWORD", token.line);
				return nullptr;
			}
			bool checkReg = token.description.find("register") == std::string::npos;
			bool checkMacro = token.description.find("macro") == std::string::npos;
			if ((buffer.empty() && (!checkReg || !checkMacro)))
			{
				processError("Wrong instruction syntax! INSTUCTION KEYWORD expected", token.line);
				return nullptr;
			}
			else if (buffer.size() == 1 && checkReg && checkMacro)
			{
				processError("Wrong instruction syntax! REGISTER or MACRO expected", token.line);
				return nullptr;
			}
			else if (buffer.size() > 1 && prevToken.value != COMMA)
			{
				processError("Wrong instruction syntax! COMMA expected", token.line);
				return nullptr;
			}
			if (!buffer.empty())
			{
				std::string value = buffer[0]->Value();
				bool checkStream = std::find(STREAM_INSTRUCTIONS.begin(), STREAM_INSTRUCTIONS.end(), value) ==
								   STREAM_INSTRUCTIONS.end();
				if (!checkReg && (std::tolower(value[0]) == 'j' || !checkStream))
				{
					processError("Instruction doesn't match its arguments! Label expected", token.line);
					return nullptr;
				}
				bool checkTypeMacro = token.description.find("type") != std::string::npos;
				if (!checkMacro && (((value == CALL || value == EXTERN) && (buffer.size() > 1 || checkTypeMacro)) ||
									(value != CALL && value != EXTERN && !checkTypeMacro)))
				{
					processError("Wrong instruction syntax! Invalid arguments", token.line);
					return nullptr;
				}
			}
			buffer.push_back(processKeyword());
		}
		else if (token.type == IDENTIFIER)
		{
			if (buffer.empty())
				return processInitialization();
			else if (buffer.size() == 1 && prevToken.type != KEYWORD)
			{
				processError("Wrong instruction syntax! INSTUCTION KEYWORD expected", token.line);
				return nullptr;
			}
			else if (buffer.size() > 1 && prevToken.value != COMMA)
			{
				processError("Wrong instruction syntax! COMMA expected", token.line);
				return nullptr;
			}
			buffer.push_back(processIdentifier());
		}
		else if (token.type == SEPARATOR && index + 1 < len)
		{
			if (buffer.empty())
			{
				processError("Wrong instruction syntax! INSTUCTION KEYWORD expected", token.line);
				return nullptr;
			}
			else if (token.value == COLON || prevToken.value == token.value ||
					 RBRACES.find(token.value) != std::string::npos)
			{
				processError("Wrong instruction syntax! Invalid SEPARATOR", token.line);
				return nullptr;
			}
			if (LBRACES.find(token.value) != std::string::npos)
			{
				Node *expr = processExpression();
				if (!expr)
					return nullptr;
				buffer.push_back(expr);
			}
			else
			{
				Token nextToken = tokens[index + 1];
				if (buffer.size() == 1)
				{
					processError("Wrong instruction syntax! IDENTIFIER, LITERAL OR REGISTER expected", token.line);
					return nullptr;
				}
				else if (nextToken.type == CONSTANT && token.description.find("Text") != std::string::npos)
				{
					processError("Wrong instruction syntax! Invalid LITERAL", token.line);
					return nullptr;
				}
				else if (nextToken.type == KEYWORD && nextToken.description.find("register") == std::string::npos &&
						 nextToken.description.find("type macro") == std::string::npos)
				{
					processError("Wrong instruction syntax! REGISTER OR EXPLICIT TYPE", token.line);
					return nullptr;
				}
			}
		}
		else if (token.type == CONSTANT)
		{
			if (buffer.empty() || (buffer.size() == 1 && prevToken.type != KEYWORD))
			{
				processError("Wrong instruction syntax! INSTUCTION KEYWORD expected", token.line);
				return nullptr;
			}
			else if (buffer.size() > 1 && prevToken.value != COMMA)
			{
				processError("Wrong instruction syntax! COMMA expected", token.line);
				return nullptr;
			}
			buffer.push_back(processLiteral());
		}
		else
		{
			buffer.clear();
			break;
		}
		if (index + 1 == len)
		{
			++index;
			break;
		}
		prevToken = tokens[index++];
		token = tokens[index];
		if ((token.type == KEYWORD && token.description.find("register") == std::string::npos &&
			 token.description.find("macro") == std::string::npos) ||
			(token.type == IDENTIFIER &&
			 (isSection(token, tokens[index + 1]) ||
			  (index + 1 < len && std::find(INITIALIZATION_NODE.begin(), INITIALIZATION_NODE.end(),
											tokens[index + 1].value) != INITIALIZATION_NODE.end()))))
			break;
	}
	if (buffer.empty() || (buffer.size() < 2 && buffer[0]->Value() != RET))
	{
		processError("Wrong instruction syntax! Invalid arguments", prevToken.line);
		return nullptr;
	}
	auto nodeValue = buffer.begin();
	Node *intsrtuction = new Node(INSTRUCTION, "INSTRUCTION_NODE", (*nodeValue)->Value(), nodeLine);
	if (buffer.size() > 1)
	{
		buffer.erase(nodeValue);
		for (const auto &val : buffer)
		{
			intsrtuction->addChild(val);
		}
	}
	return intsrtuction;
}

Node *TreeBuilder::checkChild(Node *parent, Node *node, bool isInit)
{
	if (!node || !parent)
		return node;
	if (isInit && node->Type() != INITIALIZATION)
	{

		processError("Data section must contain only initializations! Invalid instruction", node->Line());
		return nullptr;
	}
	else if (!isInit && node->Type() == INITIALIZATION)
	{
		processError("Code section must contain only instructions! Invalid initialization", node->Line());
		return nullptr;
	}
	else if (!isInit)
		return node;
	auto identifier = node->Children()[0]->Value();
	for (const auto &child : parent->Children())
	{
		if (identifier == child->Children()[0]->Value())
		{
			processError("Syntax error! Duplicated identifier in line " + std::to_string(node->Line()) + "! Identifier is already defined", child->Line());
			return nullptr;
		}
	}
	return node;
}

Node *TreeBuilder::checkDuplicates(Node *node)
{
	if (!node)
		return nullptr;
	for (const auto &child : root->Children())
	{
		if (node->Value() == child->Value())
		{
			processError("Syntax error! Duplicated label in line " + std::to_string(node->Line()) + "! Label is already defined", child->Line());
			return nullptr;
		}
	}
	return node;
}

void TreeBuilder::panicMode()
{
	while (index < len)
	{
		Token token = tokens[index], tokenNext = tokens[index + 1 == len ? index : index + 1];
		if (token.type != IDENTIFIER && token.type != KEYWORD)
		{
			++index;
			continue;
		}
		if (isSection(token, tokenNext))
			break;
		auto checkNext = std::find(INITIALIZATION_NODE.begin(), INITIALIZATION_NODE.end(), tokenNext.value);
		if (token.type == IDENTIFIER && tokenNext.type == KEYWORD && checkNext != INITIALIZATION_NODE.end())
			break;
		bool checkReg = token.description.find("register") == std::string::npos;
		bool checkMacro = token.description.find("macro") == std::string::npos;
		if (token.type == KEYWORD && checkReg && checkMacro)
			break;
		++index;
	}
}

Node *TreeBuilder::processSection()
{
	Token token = tokens[index], tokenNext = tokens[++index];
	auto checkKeyword = std::find(SECTION_NODE.begin(), SECTION_NODE.end(), token.value);
	Node *node = nullptr;
	size_t sectionLine = token.line;
	bool isInit = false;
	if (token.type == KEYWORD && checkKeyword != SECTION_NODE.end())
	{
		checkKeyword = std::find(SECTION_NODE.begin(), SECTION_NODE.end(), tokenNext.value);
		if (token.value != SECTION_NODE[0])
			processError("Wrong section declaration! KEYWORD `section` expected", sectionLine);
		else if (token.value == tokenNext.value || checkKeyword == SECTION_NODE.end())
			processError("Wrong section declaration! Valid section name expected", sectionLine);
		else
		{
			node = new Node(NONE, "SECTION_NODE", token.value + " " + tokenNext.value, sectionLine);
			isInit = tokenNext.value != TEXT;
		}
	}
	else if (token.type == IDENTIFIER)
	{
		if (tokenNext.type != SEPARATOR || tokenNext.value != COLON)
			processError("Wrong section declaration! COLON expected", sectionLine);
		else
			node = new Node(NONE, "SECTION_NODE", token.value, sectionLine);
	}
	token = tokens[++index];
	isError = false;
	while (index < len)
	{
		Node *instr = processInstruction();
		instr = checkChild(node, instr, isInit);
		if (isError)
		{
			panicMode();
			isError = false;
		}
		else if (!instr)
			break;
		else if (node)
			node->addChild(instr);
	}
	if (node && !node->hasChildren())
	{
		processError("Wrong section declaration", sectionLine);
		return nullptr;
	}
	return checkDuplicates(node);
}

void TreeBuilder::printErrors()
{
	int id_len = -1, val_len = 8;
	for (const auto &v : this->errors)
	{
		int current_id_len = std::to_string(v.id).length(),
			current_val_len = v.value.length();

		if (current_id_len > id_len)
			id_len = current_id_len;
		if (current_val_len > val_len)
			val_len = current_val_len;
	}
	id_len += 2;
	val_len += 2;
	std::string separator = "+" + std::string(id_len + 2, '-') +
							"+" + std::string(val_len + 2, '-') +
							"+\n";
	std::cout << "\n=== SYNTAX ERRORS ===\n"
			  << separator
			  << "| " << std::setw(id_len) << "#" << " | "
			  << std::setw(val_len) << "Value" << " |\n"
			  << separator;

	for (const auto &v : errors)
	{
		std::cout << "| " << std::setw(id_len) << v.id << " | "
				  << std::setw(val_len) << v.value << " |\n";
	}
	std::cout << separator;
}

TreeBuilder::TreeBuilder(std::vector<Token> &tokens)
{
	this->tokens = tokens;
	len = tokens.size();
	root = new Node(NONE, "PROGRAM_NODE");
}

void TreeBuilder::buildTree()
{
	while (index < len)
	{
		Node *section = processSection();
		if (section)
			root->addChild(section);
	}

	if (!errors.empty())
		printErrors();
	else
		root->print();
}
