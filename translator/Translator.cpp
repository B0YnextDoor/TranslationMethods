#include "Translator.h"

void Translator::printTab()
{
	for (int i = 0; i < level; ++i)
		std::cout << "    ";
}

void Translator::Printf()
{
	if (stack.empty() || stack.size() < 2)
		return;
	printTab();
	std::string str = "OUTPUT: " + stack.top()->getValue();
	stack.pop();
	std::string val = stack.top()->getValue();
	stack.pop();
	printf(str.c_str(), std::stoll(val));
}

char Translator::defineNumberType(std::string num)
{
	if (std::regex_match(num, BIN_REGEX))
		return 'b';
	else if (std::regex_match(num, HEX_REGEX))
		return 'h';
	else if (std::regex_match(num, DEC_REGEX))
		return 'd';
	return 'o';
}

void Translator::setLiteral(DataType *data, Node *node)
{
	std::string type = data->type, value = node->Value();
	bool isString = type == "char*";
	if (!isString && type.find("int") != std::string::npos)
	{
		char numType = defineNumberType(value);
		int mode = 0;
		if (numType == 'b')
		{
			value = value.substr(2);
			mode = 2;
		}
		if (type[0] == 'u')
			data->setValue(std::stoull(value, nullptr, mode));
		else
			data->setValue(std::stoll(value, nullptr, mode));
	}
	else if (!isString)
	{
		long double number;
		std::stringstream ss(value);
		ss >> number;
		data->setValue(number);
	}
	else
	{
		if (node->ValueType().find("char") == std::string::npos)
			value = char(std::stoi(value));
		data->setValue(value);
	}
}

size_t Translator::fillVariables(std::vector<Node *> values, bool isRegister)
{
	size_t i = 0;
	for (const auto &val : values)
	{
		std::string value = val->Value(), type = val->ValueType();
		if (isRegister)
		{
			variables.emplace(value, new DataType(type));
			continue;
		}
		int len = 0;
		if (!isRegister && val->Name().find("IDENTIFIER") != std::string::npos)
		{
			std::smatch match;
			if (std::regex_search(type, match, LEN_REGEX))
			{
				len = std::stoi(match[1].str());
				type = type.substr(0, type.find('[') - 1) + "*";
			}
		}
		else
			break;
		DataType *data = new DataType(type, len);
		auto children = val->Children();
		for (const auto &child : children)
			setLiteral(data, child);
		variables.emplace(value, data);
		std::cout << "INITIALIZATION TRANSALTED: " << value << " = " << data->getValue() << "\n";
		++i;
	}
	return i;
}

DataType *Translator::shiftVariable(std::string type, DataType *data, size_t shift)
{

	DataType *exprData = new DataType(type);
	if (data->isArray)
	{
		if (data->type[0] == 'i')
			exprData->setValue(*(data->ptr_int + data->index + shift));
		else if (data->type[0] == 'u')
			exprData->setValue(*(data->ptr_uint + data->index + shift));
		else if (data->type[0] != 'c')
			exprData->setValue(*(data->ptr_real + data->index + shift));
		else
			exprData->setValue(std::string(1, data->v_char[data->index + shift]));
		return exprData;
	}
	int srcTypeSize = INT_TYPES.find(type)->second;
	if (data->type[0] == 'u')
		exprData->setValue(data->v_uint >> (shift * 8));
	else if (data->type[0] == 'i')
	{
		shift *= 8;
		if (srcTypeSize == INT8_T)
			exprData->setValue(static_cast<int8_t>(data->v_int >> shift));
		else if (srcTypeSize == INT16_T)
			exprData->setValue(static_cast<int16_t>(data->v_int >> shift));
		else if (srcTypeSize == INT32_T)
			exprData->setValue(static_cast<int32_t>(data->v_int >> shift));
		else
			exprData->setValue(data->v_int >> shift);
	}
	else
	{
		if (type[0] != 'u' && type.find("int") != std::string::npos)
			exprData->type = "u" + type;
		int typeSize = REAL_TYPES.find(data->type)->second / 8;
		uint8_t bytes[typeSize] = {0};
		std::memcpy(bytes, &(data->v_real), typeSize);
		if (srcTypeSize == INT8_T)
		{
			uint8_t val;
			std::memcpy(&val, bytes + shift, sizeof(val));
			exprData->setValue(uint64_t(val));
		}
		else if (srcTypeSize == INT16_T)
		{
			uint16_t val;
			std::memcpy(&val, bytes + shift, sizeof(val));
			exprData->setValue(uint64_t(val));
		}
		else if (srcTypeSize == INT32_T)
		{
			uint32_t val;
			std::memcpy(&val, bytes + shift, sizeof(val));
			exprData->setValue(uint64_t(val));
		}
		else if (srcTypeSize == INT64_T)
		{
			uint64_t val;
			std::memcpy(&val, bytes + shift, sizeof(val));
			exprData->setValue(uint64_t(val));
		}
	}
	return exprData;
}

DataType *Translator::processExpression(Node *expr)
{
	auto children = expr->Children();
	int index = -1;
	if (expr->hasChildren())
	{
		index = 0;
		if (children[0]->Name().find("IDENTIFIER") == std::string::npos &&
			children[0]->Name().find("KEYWORD") == std::string::npos)
			index = 1;
	}
	std::string type = expr->ValueType(),
				value = index < 0 ? expr->Value() : children[index]->Value();
	auto data = variables.find(value)->second;
	size_t shift = 0;
	if (index >= 0)
		shift = std::stoull(children[index ? 0 : 1]->Value());
	DataType *exprData = shiftVariable(type, data, shift);
	return exprData;
}

void Translator::processAssigment(Node *node)
{
	auto arguments = node->Children();
	Node *dist = arguments[0], *src = arguments[1];
	auto variable = variables.find(dist->Value());
	if (src->Name().find("LITERAL") != std::string::npos)
		setLiteral(variable->second, src);
	else if (src->Name().find("IDENTIFIER") != std::string::npos || src->Name().find("KEYWORD") != std::string::npos)
	{
		auto srcVariable = variables.find(src->Value());
		variable->second = new DataType(srcVariable->second);
	}
	else if (src->Name().find("EXPRESSION") != std::string::npos)
		variable->second = processExpression(src);
	printTab();
	std::cout << "ASSIGMENT TRANSALTED: " << variable->second->toString({variable->first, "="}) << "\n";
}

int Translator::processInteruption(Node *node)
{
	auto data = variables.find("eax")->second;
	int intType = std::stoi(data->getValue());
	auto ebx = variables.find("ebx")->second;
	int exitCode = std::stoi(ebx->getValue());
	printTab();
	if (intType == 1)
	{
		std::cout << "SYS_EXIT INTERUPTION TRANSLATED: EXIT CODE = " << exitCode << "\n";
		return exitCode;
	}
	else if (intType == 4)
	{
		auto ptr_str = variables.find("ecx")->second->getValue();
		int len = std::stoi(variables.find("edx")->second->getValue());
		std::cout << "SYS_WRITE INTERUPTION TRANSALTED: ";
		for (int i = 0; i < len; ++i)
			std::cout << ptr_str[i];
		std::cout << "\n";
	}
	return exitCode;
}

void Translator::processStack(Node *node, bool type)
{
	auto var = variables.find(node->Children()[0]->Value());
	printTab();
	if (type)
	{
		stack.push(var->second);
		std::cout << "PUSH INSTRUCTION TRANSALTED: STACK ";
	}
	else
	{
		var->second = stack.top();
		stack.pop();
		std::cout << "POP INSTRUCTION TRANSALTED: " << node->Children()[0]->Value() << " = "
				  << var->second->getValue() << "\n";
		printTab();
		std::cout << "STACK ";
	}
	if (stack.empty())
		std::cout << "IS EMPTY\n";
	else
		std::cout << "TOP = " << stack.top()->getValue() << "\n";
}

std::pair<DataType *, DataType *> Translator::getArgs(std::vector<Node *> arguments)
{
	Node *arg1 = arguments[0], *arg2 = arguments.size() > 1 ? arguments[1] : nullptr;
	DataType *var1 = variables.find(arg1->Value())->second, *var2 = nullptr;
	if (!arg2)
		return {var1, var2};
	if (arg2->Name().find("LITERAL") != std::string::npos)
	{
		var2 = new DataType(var1->type);
		setLiteral(var2, arg2);
	}
	else if (arg2->Name().find("IDENTIFIER") != std::string::npos || arg2->Name().find("KEYWORD") != std::string::npos)
		var2 = variables.find(arg2->Value())->second;
	else if (arg2->Name().find("EXPRESSION") != std::string::npos)
		var2 = processExpression(arg2);
	return {var1, var2};
}

std::string Translator::toUpper(std::string str)
{
	for (auto &c : str)
		c = std::toupper(c);
	return str;
}

void Translator::processArithmetic(Node *node, std::string op)
{
	std::string inst = node->Value();
	auto data = node->Children();
	auto args = getArgs(data);
	bool isDiv = false;
	std::string search = "eax";
	if (inst == "add")
		args.first->Add(args.second);
	else if (inst == "sub")
		args.first->Add(args.second, -1);
	else if (inst == "mul" || inst == "imul")
		args.first->Mul(args.second, inst == "imul");
	else if (inst == "div" || inst == "idiv")
	{
		args.second = args.first;
		if (data[0]->Value().length() < 3)
			search = "ax";
		args.first = variables.find(search)->second;
		args.first->Div(args.second, inst == "idiv");
		isDiv = true;
	}
	else if (inst == "inc" || inst == "dec")
		args.first->Inc(inst == "inc" ? 1 : -1);
	else if (inst == "neg")
		args.first->Neg();
	else
		return;
	printTab();
	std::cout << toUpper(inst) << " INSTRUCTION TRANSLATED: ";
	bool isBin = data.size() > 1;
	if (isBin)
		std::cout << data[0]->Value() << " ";
	else if (isDiv)
		std::cout << search << " ";
	std::cout << op << " ";
	auto arg = data[isBin ? 1 : 0];
	if (arg->Name().find("EXPRESSION") != std::string::npos)
		std::cout << "[" << arg->Value() << "] = ";
	else
		std::cout << arg->Value() << " = ";
	std::cout << args.first->getFirst() << "\n";
}

void Translator::processLogic(Node *node, std::string op)
{
	std::string inst = node->Value();
	auto data = node->Children();
	auto args = getArgs(data);
	if (inst == "and")
		args.first->And(args.second);
	else if (inst == "or")
		args.first->Or(args.second);
	else if (inst == "xor")
		args.first->Xor(args.second);
	else if (inst == "not")
		args.first->Not();
	else if (inst == "shl" || inst == "sal")
		args.first->lShift(args.second, inst == "sal");
	else if (inst == "shr" || inst == "sar")
		args.first->rShift(args.second, inst == "sar");
	else
		return;
	printTab();
	std::cout << toUpper(inst) << " INSTRUCTION TRANSLATED: ";
	bool isBin = data.size() > 1;
	if (isBin)
		std::cout << data[0]->Value() << " ";
	std::cout << op << " " << data[isBin ? 1 : 0]->Value() << " = " << args.first->getValue() << "\n";
}

void Translator::processCmp(Node *node)
{
	auto data = node->Children();
	auto args = getArgs(data);
	cmpRes = args.first->cmp(args.second);
	printTab();
	std::cout << "CMP INSTRUCTION TRANSLATED: " << data[0]->Value() << " " << cmpRes << " "
			  << data[1]->Value() << "\n";
}

void Translator::processLoop()
{
	DataType *ecx = variables.find("ecx")->second;
	ecx->Inc(-1);
	printTab();
	std::cout << "DEC INSTRUCTION TRANSLATED: -- ecx = " << ecx->getValue() << "\n";
	cmpRes = ecx->cmp(new DataType(ecx->type));
	printTab();
	std::cout << "CMP INSTRUCTION TRANSLATED: ecx " << cmpRes << " 0\n";
}

int Translator::processCondition(Node *node, std::string op)
{
	if (!func)
		return 0;
	if (node->Value() == "loop")
		processLoop();
	std::string conds = CONDITIONS.find(op)->second;
	printTab();
	std::cout << toUpper(node->Value()) << " INSTRUCTION TRANSALTED: " << node->ValueType() << " - ";
	if (conds.find(cmpRes) == std::string::npos)
	{
		std::cout << "FALSE\n";
		return 1;
	}
	std::cout << "TRUE\n";
	if (node->ValueType().find("LOOP") != std::string::npos && isLoop)
	{
		nodeIdx = 0;
		return 2;
	}
	Node *label = node->Children()[0];
	auto children = func->Children();
	nodeIdx = -1;
	for (const auto &child : children)
	{
		++nodeIdx;
		if (child->Value() == label->Value())
			break;
	}
	return 2;
}

int Translator::processCall(Node *node)
{
	Node *label = node->Children()[0];
	if (label->Value() == PRINTF)
	{
		Printf();
		return 1;
	}
	for (const auto &func : functions)
	{
		if (func.first == label->Value())
		{
			this->callFunc = func.second;
			break;
		}
	}
	printTab();
	std::cout << "CALL ";
	return 3;
}

int Translator::processInstruction(Node *node)
{
	auto instruction = INSTRUCTIONS.find(node->Value());
	if (instruction == INSTRUCTIONS.end())
		return 1;
	std::string inst = instruction->second;
	if (inst == "call")
		return processCall(node);
	else if (instruction->first[0] == 'j' || instruction->first == "loop")
		return processCondition(node, inst);
	else if (inst == "=")
		processAssigment(node);
	else if (ARITHMETIC_INST.find(inst) != std::string::npos)
		processArithmetic(node, inst);
	else if (BIT_INST.find(inst) != std::string::npos)
		processLogic(node, inst);
	else if (inst == "cmp")
		processCmp(node);
	else if (inst == "push" || inst == "pop")
		processStack(node, inst == "push");
	else if (inst == "int")
		return processInteruption(node);
	else if (inst == "ret")
	{
		--level;
		printTab();
		std::cout << "RET INSTRUCTION TRANSALTED: RETURN FROM " << this->func->Value() << " FUNCTION\n";
	}
	return 1;
}

int Translator::processFunction(Node *func, bool isMain = false)
{
	if (func->ValueType().find("FUNCTION") != std::string::npos)
	{
		std::cout << "FUNCTION " << (isMain ? "MAIN" : func->Value()) << ":\n";
		this->func = func;
	}
	else
	{
		printTab();
		++level;
		std::cout << func->ValueType() << " : " << func->Value() << "\n";
		if (func->ValueType().find("LOOP") != std::string::npos)
			isLoop = true;
		else
			isLoop = false;
	}
	auto children = func->Children();
	int status;
	size_t i = 0, len = children.size();
	while (i < len)
	{
		auto node = children[i];
		if (node->Name().find("INSTRUCTION") != std::string::npos)
		{
			status = processInstruction(node);
			if (!status)
				return 0;
			else if (status == 2)
			{
				i = nodeIdx - 1;
				if (i + 1 >= len || (!isLoop && children[i + 1]->Name().find("SECTION") == std::string::npos))
					return i;
			}
			else if (status == 3)
			{
				auto prevFunc = this->func;
				++level;
				if (!processFunction(this->callFunc))
					return 0;
				this->func = prevFunc;
			}
		}
		else
		{
			auto prevFunc = this->func;
			status = processFunction(node);
			if (status + 1 < len && children[status + 1]->Name().find("SECTION") != std::string::npos)
				i = status;
			else if (!status)
				return 0;
			--level;
			this->func = prevFunc;
		}
		++i;
	}
	return isMain ? 0 : 1;
}

void Translator::translate(std::vector<Node *> registers, std::vector<Node *> identifiers)
{
	fillVariables(registers, true);
	size_t start = fillVariables(identifiers, false),
		   len = identifiers.size();
	while (start < len)
	{
		Node *node = identifiers[start++];
		std::string type = node->ValueType();
		if (type.find("MAIN") != std::string::npos)
			processFunction(node, true);
		else if (type.find("FUNCTION") != std::string::npos)
			functions.emplace(node->Value(), node);
	}
	std::cout << "PROGRAM FINISHED\n";
}
