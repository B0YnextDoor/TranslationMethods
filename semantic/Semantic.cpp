#include "Semantic.h"

void Semantic::processError(std::string message, size_t line)
{
	message += " in line " + std::to_string(line) + "!";
	errors.push_back(Token(errors.size() + 1, ERROR, message, "", line));
}

size_t Semantic::processTextLiteral(int typeSize, Node *node)
{
	if (typeSize != INT8_T)
		processError("Invalid TEXT LITERAL type declaration", node->Line());
	std::string nodeValue = node->Value();
	size_t len = nodeValue.length() - 2;
	node->ValueType("char [" + std::to_string(len) + "]");
	node->Value(nodeValue.substr(1, len));
	return len;
}

char Semantic::defineNumberType(std::string num)
{
	if (std::regex_match(num, BIN_REGEX))
		return 'b';
	else if (std::regex_match(num, OCT_REGEX))
		return 'o';
	else if (std::regex_match(num, HEX_REGEX))
		return 'h';
	else if (std::regex_match(num, DEC_REGEX))
		return 'd';
	return 'c';
}

std::string Semantic::transformNumber(std::string num, char type)
{
	size_t n = num.length();
	if (type == 'd')
		return num;
	else if (type == 'b' && num[n - 1] == 'b' || num[n - 1] == 'B')
		return "0B" + num.substr(0, n - 1);
	else if (type == 'h' && (num[n - 1] == 'h' || num[n - 1] == 'H'))
		return "0x" + num.substr(0, n - 1);
	else if (type == 'o')
	{
		if (num[1] == 'q' || num[1] == 'Q' || num[1] == 'o' || num[1] == 'O')
			return "0" + num.substr(2);
		return "0" + num.substr(0, n - 1);
	}
	else if (type == 'c')
		return num.substr(1, num.length() - 2);
	return num;
}

std::string Semantic::checkFitFloat(std::string num, int typeSize, size_t line, char type)
{
	if ((num[0] != '-' && num.length() > 25) || (num[0] == '-' && num.length() > 26))
	{
		processError("Invalid type for a FLOATING POINT VALUE! Too big LITERAL", line);
		return "";
	}
	else if (type == 'c')
		num = std::to_string((int)num[0]);
	long double number, error;
	std::stringstream ss(num);
	ss >> number;
	if (typeSize == 32 && number >= -std::numeric_limits<float>::max() && number <= std::numeric_limits<float>::max())
	{
		float fValue = static_cast<float>(number);
		error = std::fabs(fValue - number) / std::fabs(number);
		if (error <= std::numeric_limits<float>::epsilon())
			return "float";
	}
	else if (typeSize == 64 && number >= -std::numeric_limits<double>::max() &&
			 number <= std::numeric_limits<double>::max())
	{
		double dValue = static_cast<double>(number);
		error = std::fabs(dValue - number) / std::fabs(number);
		if (error <= std::numeric_limits<double>::epsilon())
			return "double";
	}
	else if (typeSize == 80 && number >= -std::numeric_limits<long double>::max() &&
			 number <= std::numeric_limits<long double>::max())
	{
		long double ldValue = static_cast<long double>(number);
		error = std::fabs(ldValue - number) / std::fabs(number);
		if (error <= std::numeric_limits<long double>::epsilon())
			return "long double";
	}
	processError("Invalid type for a FLOATING POINT VALUE! Too big LITERAL", line);
	return "";
}

std::string Semantic::checkFitInt(std::string num, int typeSize, size_t line, char numType, bool isSigned = true)
{
	if ((num[0] != '-' && num.length() > 19) || (num[0] == '-' && num.length() > 20))
	{
		processError("Invalid type for a NUMBER VALUE! Too big LITERAL", line);
		return "";
	}
	std::string type = "int" + std::to_string(typeSize) + "_t";
	try
	{
		int mode = 0;
		if (numType == 'b')
		{
			num = num.substr(2);
			mode = 2;
		}
		else if (numType == 'c')
			num = std::to_string((int)num[0]);
		if (isSigned)
		{
			long long number = std::stoll(num, nullptr, mode),
					  min = -(1LL << (typeSize - 1)),
					  max = (1LL << (typeSize - 1)) - 1;
			if (number >= min && number <= max)
				return type;
		}
		else
		{
			unsigned long long number = std::stoull(num, nullptr, mode),
							   min = 0,
							   max = (1LL << typeSize) - 1;
			if (number >= 0 && number <= max)
				return "u" + type;
		}
		if (isSigned && num[0] != '-')
			return checkFitInt(num, typeSize, line, numType, false);
		processError("Invalid type for a " + type + " NUMBER VALUE! Too big LITERAL", line);
		return "";
	}
	catch (std::out_of_range const &ex)
	{
		if (isSigned && num[0] != '-')
			return checkFitInt(num, typeSize, line, numType, false);
		processError("Invalid type for a " + type + " NUMBER VALUE! Too big LITERAL", line);
		return "";
	}
	catch (...)
	{
		processError("Invalid type for a " + type + " NUMBER VALUE! Too big LITERAL", line);
		return "";
	}
}

size_t Semantic::processNumberLiteral(int typeSize, Node *node, bool isFloatingPoint)
{
	char numType = defineNumberType(node->Value());
	std::string number = transformNumber(node->Value(), numType);
	std::string vType = isFloatingPoint ? checkFitFloat(number, typeSize, node->Line(), numType)
										: checkFitInt(number, typeSize, node->Line(), numType);
	node->ValueType(vType);
	if (vType != "")
		node->Value(number);
	return 1;
}

std::vector<std::pair<std::string, int>> Semantic::getTypes(bool isFloatingPoint)
{
	auto typesMap = isFloatingPoint ? REAL_TYPES : INT_TYPES;
	std::vector<std::pair<std::string, int>> types;
	for (const auto &type : typesMap)
		types.push_back(type);
	std::sort(types.begin(), types.end(), [](const auto &a, const auto &b)
			  { return a.second < b.second; });
	return types;
}

void Semantic::defineLiteralType(Node *node)
{
	std::string val = node->Value();
	bool isFound = false;
	bool isFloatingPoint = std::regex_match(val, POINT_REGEX);
	size_t errorCount = 0;
	auto types = getTypes(isFloatingPoint);
	for (const auto &pair : types)
	{
		processNumberLiteral(pair.second, node, isFloatingPoint);
		if (!node->ValueType().empty())
		{
			isFound = true;
			break;
		}
		else
			++errorCount;
	}

	if (!isFound)
		--errorCount;
	for (size_t i = 0; i < errorCount; ++i)
		errors.pop_back();
}

void Semantic::processDefine(Node *node)
{
	if (node->Value() != EQU)
		return;
	auto children = node->Children();
	Node *identifier = children[0];
	Node *literal = children[1];
	std::string val = literal->Value();
	std::string type = "";
	if (val[0] == '\'' || val[0] == '"')
	{
		size_t count = processTextLiteral(INT8_T, literal);
		type = "char [" + std::to_string(count) + "]";
	}
	else
	{
		defineLiteralType(literal);
		type = literal->ValueType();
	}
	identifier->ValueType(type);
	identifiers.push_back(identifier);
}

void Semantic::processInitialization(Node *node)
{
	auto keyword = KEYWORDS.find(node->Value());
	std::smatch match;
	if (!std::regex_search(keyword->second, match, SIZE_REGEX))
		return processDefine(node);
	int typeSize = std::stoi(match.str()) * 8;
	auto children = node->Children();
	size_t n = children.size();
	size_t count = 0;
	Node *identifier = children[0];
	bool isFloatingPoint = false;
	bool isString = false;
	for (size_t i = 1; i < n; ++i)
	{
		auto val = children[i]->Value();
		if (val[0] == '\'' || val[0] == '"')
		{
			count += processTextLiteral(typeSize, children[i]);
			isString = true;
		}
		else
		{
			isFloatingPoint = std::regex_match(val, POINT_REGEX) || typeSize > INT64_T;
			if (isFloatingPoint && (isString || typeSize < INT32_T))
			{
				processError("Invalid type for a FLOATING POINT LITERAL", node->Line());
				continue;
			}
			count += processNumberLiteral(typeSize, children[i], isFloatingPoint);
		}
	}
	std::string arr_size = " [" + std::to_string(count) + "]";
	if (isString)
		identifier->ValueType("char" + arr_size);
	else
		identifier->ValueType(children[1]->ValueType() + (count > 1 ? arr_size : ""));
	identifiers.push_back(identifier);
}

Node *Semantic::findNode(std::string value, bool isRoot = false)
{
	auto nodes = isRoot ? root->Children() : identifiers;
	for (const auto &node : nodes)
	{
		if (node->Value() == value)
			return node;
	}
	return nullptr;
}

void Semantic::processRegister(Node *node, Node *prevNode, std::string type)
{
	auto keyword = KEYWORDS.find(node->Value());
	std::smatch match;
	if (!std::regex_search(keyword->second, match, SIZE_REGEX))
	{
		processError("Identifier must be initialized! Uninitializaed undentifier", node->Line());
		return;
	}
	std::string valueType = "int" + match.str() + "_t";
	int typeSize = std::stoi(match.str());
	node->ValueType(valueType);
	if (type.empty())
	{
		if (prevNode->Name().find("LITERAL") == std::string::npos)
			return;
		std::string preValue = prevNode->Value();
		bool isFloatingPoint = std::regex_match(preValue, POINT_REGEX);
		processNumberLiteral(typeSize, prevNode, isFloatingPoint);
		std::string preValueType = prevNode->ValueType();
		if (!preValueType.empty() && preValueType != valueType)
		{
			if (isFloatingPoint)
			{
				if (typeSize >= INT32_T)
					node->ValueType(preValueType);
				else
					processError("Invalid type in instruction! FLOATING POINT type expected", node->Line());
				return;
			}
			node->ValueType(preValueType);
		}
	}
	else if (type != valueType)
	{
		auto checkType = REAL_TYPES.find(type);
		int prevTypeSize;
		if (checkType == REAL_TYPES.end())
			prevTypeSize = INT_TYPES.find(type)->second;
		else
			prevTypeSize = checkType->second;
		if (prevTypeSize < typeSize)
			processError("Invalid type in instruction! Expected " + type + " type", node->Line());
	}
}

void Semantic::processIdentifier(Node *node, Node *prevNode, std::string type)
{
	Node *identifier = findNode(node->Value());
	if (!identifier)
		return processRegister(node, prevNode, type);
	std::string valueType = identifier->ValueType();
	if (valueType.empty())
		return;
	auto checkReal = REAL_TYPES.find(valueType);
	auto checkInt = INT_TYPES.find(valueType);
	int typeSize;
	bool isString = false;
	if (checkReal != REAL_TYPES.end())
		typeSize = checkReal->second;
	else if (checkInt != INT_TYPES.end())
		typeSize = checkInt->second;
	else
	{
		valueType = "uint8_t";
		typeSize = INT8_T;
		isString = true;
	}
	node->ValueType(valueType);
	if (type.empty())
	{
		if (prevNode->Name().find("LITERAL") == std::string::npos)
			return;
		std::string preValue = prevNode->Value();
		bool isFloatingPoint = std::regex_match(preValue, POINT_REGEX);
		processNumberLiteral(typeSize, prevNode, isFloatingPoint);
		std::string preValueType = prevNode->ValueType();
		if (!preValueType.empty() && preValueType != valueType)
			processError("Invalid type in instruction! Expected " + valueType + " type", node->Line());
	}
	else if (type != valueType)
	{
		checkReal = REAL_TYPES.find(type);
		int prevTypeSize;
		if (checkReal == REAL_TYPES.end())
			prevTypeSize = INT_TYPES.find(type)->second;
		else
			prevTypeSize = checkReal->second;
		bool checkKeyword = KEYWORDS.find(prevNode->Value()) != KEYWORDS.end();
		if (prevTypeSize < typeSize)
			processError("Invalid type in instruction! Expected " + type + " type", node->Line());
		else if (prevTypeSize == typeSize && checkKeyword)
			prevNode->ValueType(valueType);
	}
	if (isString)
		node->ValueType("char*");
}

void Semantic::processLiteral(Node *node, Node *prevNode, std::string type)
{
	std::string nodeValue = node->Value();
	if (type.empty())
		defineLiteralType(node);
	else
	{
		std::string preValueType = prevNode->ValueType();
		auto checkReal = REAL_TYPES.find(preValueType);
		int prevTypeSize;
		bool isInt = checkReal == REAL_TYPES.end();
		if (isInt)
			prevTypeSize = INT_TYPES.find(preValueType)->second;
		else
			prevTypeSize = checkReal->second;
		bool isFloatingPoint = std::regex_match(nodeValue, POINT_REGEX) || prevTypeSize > INT64_T;
		processNumberLiteral(prevTypeSize, node, isFloatingPoint);
		std::string valueType = node->ValueType();
		if (!valueType.empty() && preValueType != valueType)
		{
			if (!isInt && !isFloatingPoint)
				node->ValueType(preValueType);
			else if (isInt && !isFloatingPoint && valueType[0] == 'u')
				prevNode->ValueType(valueType);
			else
				processError("Invalid type in instruction! Expected " + preValueType + " type", node->Line());
		}
	}
}

void Semantic::processChildren(std::vector<Node *> children, std::string nodeValue)
{
	std::string type = "";
	Node *prevChild = children[0];
	for (const auto &child : children)
	{
		std::string name = child->Name();
		if (name.find("KEYWORD") != std::string::npos)
		{
			auto keyword = KEYWORDS.find(child->Value());
			if (keyword->second.find("register") != std::string::npos)
			{
				processRegister(child, prevChild, type);
				type = child->ValueType();
				if (type.empty())
					return;
			}
		}
		else if (name.find("EXPRESSION") != std::string::npos)
		{
			processExpression(child, prevChild, type);
			type = child->ValueType();
			if (type.empty())
				return;
		}
		else if (name.find("IDENTIFIER") != std::string::npos)
		{
			if (type.empty() && !nodeValue.empty() && nodeValue != PUSH && nodeValue != CMP)
			{
				processError("Invalid combination of instruction and operands! Attempting to change the value of a constant", child->Line());
				return;
			}
			processIdentifier(child, prevChild, type);
			type = child->ValueType();
			if (type.empty())
				return;
		}
		else
			processLiteral(child, prevChild, type);
		prevChild = child;
	}
}

bool Semantic::checkExpression(Node *node)
{
	if (!node->hasChildren())
		return true;
	auto children = node->Children();
	for (const auto &child : children)
	{
		if (child->Name().find("IDENTIFIER") != std::string::npos)
			return true;
	}
	return false;
}

void Semantic::processExpression(Node *node, Node *prevNode, std::string type)
{
	std::string valueType;
	if (!node->hasChildren())
	{
		processIdentifier(node, prevNode, "");
		valueType = node->ValueType();
	}
	else
	{
		auto children = node->Children();
		processChildren(children, "");
		valueType = children[0]->ValueType();
	}
	if (valueType.empty())
		return;
	auto checkReal = REAL_TYPES.find(valueType);
	int typeSize;
	if (checkReal == REAL_TYPES.end())
		typeSize = INT_TYPES.find(valueType)->second;
	else
		typeSize = checkReal->second;
	bool hasMacro = false;
	if (prevNode->Name().find("KEYWORD") != std::string::npos &&
		KEYWORDS.at(prevNode->Value()).find("type macro") != std::string::npos)
	{
		int macroSize = TYPE_MACRO.at(prevNode->Value());
		if (macroSize > typeSize)
		{
			processError("Can't provide explicit type conversion! Invalid type macro", prevNode->Line());
			return;
		}
		else
		{
			typeSize = macroSize;
			valueType = "int" + std::to_string(typeSize) + "_t";
			hasMacro = true;
		}
	}
	node->ValueType(valueType);
	if (type != valueType)
	{
		checkReal = REAL_TYPES.find(type);
		int prevTypeSize;
		bool isInt = checkReal == REAL_TYPES.end();
		if (isInt)
			prevTypeSize = INT_TYPES.find(type)->second;
		else
			prevTypeSize = checkReal->second;
		if (checkExpression(node))
			node->ValueType(type);
		else if (prevTypeSize < typeSize)
			processError("Invalid type in instruction! Expected " + type + " type", node->Line());
	}
}

void Semantic::processStream(Node *node)
{
	std::string nodeValue = node->Value(), type = "FUNCTION",
				identifier = node->Children()[0]->Value();
	if (identifier == PRINTF)
		return;
	Node *parent = findNode(identifier, true);
	if (!parent)
	{
		processError("Label must be initialized! Uninitializaed label", node->Line());
		return;
	}
	if (nodeValue == GLOBAL)
		type = "MAIN " + type;
	auto check = findNode(parent->Value());
	if (nodeValue == CALL || nodeValue == GLOBAL || nodeValue == JMP)
	{
		parent->ValueType(type);
		node->ValueType(type + " CALL");
		if (!check)
		{
			processSection(parent);
			identifiers.push_back(parent);
		}
		return;
	}
	if (parent->Line() < node->Line())
	{
		type = "LOOP";
		parent->ValueType(type);
		node->ValueType(type + " CONDTITION CHECK");
		if (!check)
			identifiers.push_back(parent);
		return;
	}
	processSection(parent);
	std::smatch match;
	auto keyword = KEYWORDS.find(nodeValue);
	if (keyword == KEYWORDS.end() || !std::regex_search(keyword->second, match, CONDITION_REGEX))
		return;
	type = "IF " + match[0].str();
	parent->ValueType(type + " BLOCK");
	node->ValueType(type + "  CONDITION CHECK");
	identifiers.push_back(parent);
}

void Semantic::processInstruction(Node *node)
{
	std::string keyword = node->Value();
	if (keyword == EXTERN || !node->hasChildren())
		return;
	auto checkStream = std::find(STREAM_INSTRUCTIONS.begin(), STREAM_INSTRUCTIONS.end(), keyword);
	if ((checkStream != STREAM_INSTRUCTIONS.end() && keyword != INT) || std::tolower(keyword[0]) == 'j')
		processStream(node);
	else
		processChildren(node->Children(), node->Value());
}

void Semantic::processNode(Node *node)
{
	if (node->Type() == INSTRUCTION)
		return processInstruction(node);
	return processInitialization(node);
}

void Semantic::processSection(Node *node)
{
	if (!node->hasChildren())
	{
		processError("Section mustn't be empty! Wrong section declaration", node->Line());
		return;
	}
	bool isInit = !node->Value().empty() && node->Value().find(SECTION_KWD) != std::string::npos &&
				  node->Value() != SECTION_TEXT;
	auto children = node->Children();
	for (const auto &child : children)
	{
		if (isInit && child->Type() != INITIALIZATION)
			processError("Data section must contain only initializations! Invalid instruction", child->Line());
		else if (!isInit && child->Type() == INITIALIZATION)
			processError("Code section must contain only instructions! Invalid initialization", child->Line());
		processNode(child);
	}
}

void Semantic::printErrors()
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
	std::cout << "\n=== SEMANTIC ERRORS ===\n"
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

Semantic::Semantic(Node *tree)
{
	root = tree;
}

void Semantic::analize()
{
	auto children = root->Children();

	for (const auto &child : children)
	{
		if (!child->ValueType().empty())
			continue;
		std::string value = child->Value();
		bool hasSection = !value.empty() && value.find(SECTION_KWD) == std::string::npos;
		processSection(child);
		if (child->ValueType().empty() && hasSection)
		{
			child->ValueType("FUNCTION");
			identifiers.push_back(child);
		}
	}

	if (!errors.empty())
		printErrors();
	else
	{
		root->print();
		std::cout << "\nIdentifiers\n";
		for (const auto &val : identifiers)
		{
			std::cout << val->Name() << ": " << val->Value() << " - " << val->ValueType() << "\n";
		}
	}
}
