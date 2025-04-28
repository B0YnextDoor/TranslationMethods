#include "Lexer.h"
#include "Constants.h"

std::string Lexer::trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

bool Lexer::isConstant(const std::string &str, char type, size_t lineNumber)
{
	if (type == 'd' && !std::regex_match(str, DEC_REGEX))
	{
		processError("Wrong decimal number literal in line " + std::to_string(lineNumber) + "!");
		return false;
	}
	else if (type == 'x' && !std::regex_match(str, HEX_REGEX))
	{
		processError("Wrong hexadecimal number literal in line " + std::to_string(lineNumber) + "!");
		return false;
	}
	else if (type == 'o' && !std::regex_match(str, OCT_REGEX))
	{
		processError("Wrong octal number literal in line " + std::to_string(lineNumber) + "!");
		return false;
	}
	else if (type == 'b' && !std::regex_match(str, BIN_REGEX))
	{
		processError("Wrong binary number literal in line " + std::to_string(lineNumber) + "!");
		return false;
	}
	return true;
}

bool Lexer::isIdentificator(const std::string &str, size_t lineNumber)
{
	if (std::regex_match(str, IDENTIFIER_REGEX))
		return true;
	processError("Inavlid identifier `" + str + "` in line " + std::to_string(lineNumber) + "!");
	return false;
}

void Lexer::addSeparator(std::string sep)
{
	if (separatorLexems.find(sep) == separatorLexems.end())
		separatorLexems[sep] = {++counter,
								SEPARATOR,
								sep,
								SEPARATORS.at(sep)};
	else
		separatorLexems[sep].id = ++counter;
}

void Lexer::processError(std::string error = "Unknown character")
{
	size_t id = errors.size() + 1;
	errors[std::to_string(id)] = {id, ERROR, error};
}

void Lexer::processComment(std::string str, size_t lineNumber, size_t start)
{
	std::string comm = str.substr(start);
	if (comm.length())
	{
		size_t id = comments.size() + 1;
		comments[std::to_string(id)] = {id, COMMENT, comm, "Comment string in line " + std::to_string(lineNumber)};
	}
}

size_t Lexer::processTextLexeme(std::string str, size_t lineNumber, size_t start)
{
	size_t len = str.length();
	char endChar = str[start++];
	std::string buffer(1, endChar);
	while (start < len)
	{
		buffer += str[start++];
		if (str[start - 1] == endChar)
			break;
	}
	if (buffer.length() < 2 || str[start - 1] != endChar)
	{
		processError("Wrong string literal in line " + std::to_string(lineNumber) + ". `" + endChar + "` expected!");
		return len;
	}
	else if (constantLexems.find(buffer) == constantLexems.end())
		constantLexems[buffer] = {++counter, CONSTANT, buffer, "Text literal"};
	else
		constantLexems[buffer].id = ++counter;
	return start;
}

char Lexer::defineNumberType(char symb)
{
	if (symb == 'x' || symb == 'X')
		return 'x';
	if (symb == 'b' || symb == 'B')
		return 'b';
	if (symb == 'o' || symb == 'O' || symb == 'q' || symb == 'Q')
		return 'o';
	return 'd';
}

size_t Lexer::processNumberLexeme(std::string str, size_t lineNumber, size_t start)
{
	size_t len = str.length();
	std::string buffer(1, str[start++]);
	char type = 'd';
	bool hasDot = false, hasExp = false;
	if (str[start - 1] == '0' && start < len)
	{
		type = defineNumberType(str[start]);
		if (type != 'd')
			buffer += str[start++];
	}
	while (start < len && (std::isalnum(str[start]) || str[start] == '.' || str[start] == 'e' || str[start] == 'E'))
	{
		if (str[start] == '.')
		{
			if (type != 'd' || hasDot)
			{
				processError("Wrong floating point literal in line " + std::to_string(lineNumber) + "!");
				return len;
			}
			hasDot = true;
		}
		if (str[start] == 'e' || str[start] == 'E')
		{
			if (hasExp)
			{
				processError("Wrong floating point literal in line " + std::to_string(lineNumber) + "!");
				return len;
			}
			hasExp = true;
		}
		buffer += str[start++];
	}
	std::string end_types = "hqob";
	char lastChar = str[start - 1];
	if (end_types.find(std::tolower(lastChar)) != std::string::npos)
		type = defineNumberType(lastChar);
	if (isConstant(buffer, type, lineNumber))
	{
		if (constantLexems.find(buffer) == constantLexems.end())
		{
			std::string description = ((type == 'x') ? "Hexadecimal" : (type == 'b')	? "Binary"
																   : (type == 'o')		? "Octal"
																   : (hasDot || hasExp) ? "Floating Point"
																						: "Integer");
			constantLexems[buffer] = {++counter, CONSTANT, buffer, description + " number literal"};
		}
		else
			constantLexems[buffer].id = ++counter;
		return start;
	}
	return len;
}

size_t Lexer::processSeparator(std::string str, size_t lineNumber, size_t start)
{
	size_t i = start;
	char sep = str[i];
	std::string newLex(1, sep);
	bool freeLexeme = separatorLexems.find(newLex) == separatorLexems.end();
	if (sep == ',' || sep == ':' || sep == ')' || sep == ']' || sep == '}')
	{
		addSeparator(newLex);
		return start + 1;
	}
	char second = sep == '(' ? ')' : sep == '[' ? ']'
												: '}';
	size_t len = str.length();
	while (i < len && sep != second)
	{
		sep = str[i++];
	}
	if (sep != second)
	{
		processError("Expected closing bracket `" + std::string(1, second) + "` in line " + std::to_string(lineNumber) + "!");
		return len;
	}
	addSeparator(newLex);
	return start + 1;
}

size_t Lexer::processOperator(std::string str, size_t lineNumber, size_t start)
{
	std::string operatorLexeme(1, str[start++]);
	if (OPERATORS.find(operatorLexeme) == OPERATORS.end())
		operatorLexeme += str[start++];
	if (operatorLexems.find(operatorLexeme) == operatorLexems.end())
		operatorLexems[operatorLexeme] = {++counter,
										  OPERATOR, operatorLexeme, OPERATORS.at(operatorLexeme)};
	else
		operatorLexems[operatorLexeme].id = ++counter;
	return start;
}

size_t Lexer::processKeyword(std::string str, size_t lineNumber, size_t start)
{
	size_t len = str.length();
	std::string lexem(1, std::tolower(str[start++]));
	while (start < len && (str[start] == '_' || std::isalnum(str[start])))
	{
		lexem += std::tolower(str[start++]);
	}
	if (KEYWORDS.find(lexem) != KEYWORDS.end())
	{
		if (keywordLexems.find(lexem) == keywordLexems.end())
			keywordLexems[lexem] = {++counter,
									KEYWORD, lexem, KEYWORDS.at(lexem)};
		else
			keywordLexems[lexem].id = ++counter;
		return start;
	}
	else if (isIdentificator(lexem, lineNumber))
	{
		if (identifierLexems.find(lexem) == identifierLexems.end())
			identifierLexems[lexem] = {++counter, IDENTIFIER, lexem, "Identifier"};
		else
			identifierLexems[lexem].id = ++counter;
		return start;
	}
	return len;
}

std::vector<Token> Lexer::mapToVector(std::unordered_map<std::string, Token> map)
{
	auto result = std::vector<Token>();
	for (const auto &[_, value] : map)
		result.push_back(value);
	std::sort(result.begin(), result.end(), [](Token a, Token b)
			  { return a.id < b.id; });
	return result;
}

void Lexer::printVector(std::string name, std::vector<Token> values)
{
	int id_len = -1, val_len = 4, descr_len = 8;
	for (const auto &v : values)
	{
		int current_id_len = std::to_string(v.id).length(),
			current_val_len = v.value.length(), current_descr_len = v.description.length();

		if (current_id_len > id_len)
			id_len = current_id_len;
		if (current_val_len > val_len)
			val_len = current_val_len;
		if (current_descr_len > descr_len)
			descr_len = current_descr_len;
	}
	id_len += 2;
	val_len += 2;
	descr_len += 2;
	std::cout << "\n=== " << name << " ===\n";
	std::string separator = "+" + std::string(id_len + 2, '-') +
							"+" + std::string(val_len + 2, '-') +
							"+";
	if (name[0] != 'E')
		separator += std::string(descr_len + 2, '-') + "+";
	std::cout << separator << "\n";
	std::cout << "| " << std::setw(id_len) << "#" << " | "
			  << std::setw(val_len) << "Value" << " |";
	if (name[0] != 'E')
		std::cout << std::setw(descr_len) << "Description" << " |\n";
	else
		std::cout << "\n";
	std::cout << separator << "\n";

	for (const auto &v : values)
	{
		std::cout << "| " << std::setw(id_len) << v.id << " | "
				  << std::setw(val_len) << v.value << " |";
		if (name[0] != 'E')
			std::cout << std::setw(descr_len) << v.description << " |\n";
		else
			std::cout << "\n";
	}
	std::cout << separator << "\n";
}

void Lexer::printTables()
{
	printVector("IDENTIFIERS", mapToVector(identifierLexems));
	printVector("KEYWORDS", mapToVector(keywordLexems));
	printVector("CONSTANTS", mapToVector(constantLexems));
	printVector("OPERATORS", mapToVector(operatorLexems));
	printVector("SEPARATORS", mapToVector(separatorLexems));
	printVector("COMMENTS", mapToVector(comments));
	printVector("ERRORS", mapToVector(errors));
}

void Lexer::analyzeLine(std::string line, size_t lineNumber)
{
	size_t len = line.length();
	if (!len)
		return;
	size_t i = 0;
	while (i < len)
	{
		char symb = line[i];
		if (std::isspace(symb))
		{
			++i;
			continue;
		}
		if (symb == ';')
		{
			processComment(line, lineNumber, i);
			break;
		}
		if (symb == '\'' || symb == '"')
		{
			i = processTextLexeme(line, lineNumber, i);
			continue;
		}
		if (i != 0 && (std::isdigit(symb) || (symb == '0' && (i + 1 < len && (line[i + 1] == 'x' || line[i + 1] == 'b' || line[i + 1] == 'o' || line[i + 1] == 'q')))))
		{
			i = processNumberLexeme(line, lineNumber, i);
			continue;
		}
		std::string lexem(1, symb);
		if (SEPARATORS.find(lexem) != SEPARATORS.end())
		{
			i = processSeparator(line, lineNumber, i);
			continue;
		}
		if (OPERATORS.find(lexem) != OPERATORS.end() || OPERATORS.find(lexem + line[i + 1]) != OPERATORS.end())
		{
			i = processOperator(line, lineNumber, i);
			continue;
		}
		if (symb == '.' || symb == '_' || std::isalnum(symb))
		{
			i = processKeyword(line, lineNumber, i);
			continue;
		}

		processError("Unknown character `" + std::string(1, symb) + "` in line " + std::to_string(lineNumber) + "!");
		break;
	}
}

void Lexer::analyzeFile(const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Can't open the file:" << filename << "\n";
		return;
	}

	std::string line;
	size_t i = 0;
	while (getline(file, line))
	{
		analyzeLine(trim(line), i + 1);
		++i;
	}
	file.close();
	printTables();
}
