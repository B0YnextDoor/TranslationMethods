#include "Lexer.h"

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
	std::string error = "";
	if (type == 'd' && !std::regex_match(str, DEC_REGEX))
		error = "decimal";
	else if (type == 'x' && !std::regex_match(str, HEX_REGEX))
		error = "hexadecimal";
	else if (type == 'o' && !std::regex_match(str, OCT_REGEX))
		error = "octal";
	else if (type == 'b' && !std::regex_match(str, BIN_REGEX))
		error = "binary";
	if (!error.length())
		return true;
	processError("Wrong " + error + " number literal", lineNumber);
	return false;
}

bool Lexer::isIdentificator(const std::string &str, size_t lineNumber)
{
	return std::regex_match(str, IDENTIFIER_REGEX);
}

void Lexer::processError(std::string error, size_t lineNumber)
{
	error += " in line " + std::to_string(lineNumber) + "!";
	errors.push_back(Token(errors.size() + 1, ERROR, error, "", lineNumber));
}

void Lexer::processComment(std::string str, size_t lineNumber, size_t start)
{
	std::string comm = str.substr(start);
	if (comm.length())
		comments.push_back(Token(comments.size() + 1, COMMENT, comm,
								 "Comment string in line " + std::to_string(lineNumber)));
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
		processError("Wrong string literal! Expected `" + std::string(1, endChar) + "`", lineNumber);
		return len;
	}
	else
		tokens.push_back(Token(tokens.size() + 1, CONSTANT, buffer, "Text literal", lineNumber));
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
				processError("Wrong floating point literal", lineNumber);
				return len;
			}
			hasDot = true;
		}
		if (str[start] == 'e' || str[start] == 'E')
		{
			if (hasExp)
			{
				processError("Wrong floating point literal", lineNumber);
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
		std::string description = ((type == 'x') ? "Hexadecimal" : (type == 'b')	? "Binary"
															   : (type == 'o')		? "Octal"
															   : (hasDot || hasExp) ? "Floating Point"
																					: "Integer");
		tokens.push_back(Token(tokens.size() + 1, CONSTANT, buffer, description + " number literal", lineNumber));
		return start;
	}
	return len;
}

size_t Lexer::processSeparator(std::string str, size_t lineNumber, size_t start)
{
	size_t i = start;
	char sep = str[i];
	std::string newLex(1, sep);
	size_t id = tokens.size() + 1;
	if (sep == ',' || sep == ':' || sep == ')' || sep == ']')
	{
		tokens.push_back(Token(id, SEPARATOR, newLex, SEPARATORS.at(newLex), lineNumber));
		return start + 1;
	}
	char second = sep == '(' ? ')' : ']';
	size_t len = str.length();
	while (i < len && sep != second)
	{
		sep = str[i++];
	}
	if (sep != second)
	{
		processError("Expected closing bracket `" + std::string(1, second) + "`", lineNumber);
		return len;
	}
	tokens.push_back(Token(id, SEPARATOR, newLex, SEPARATORS.at(newLex), lineNumber));
	return start + 1;
}

size_t Lexer::processOperator(std::string str, size_t lineNumber, size_t start)
{
	std::string operatorLexeme(1, str[start++]);
	std::string doubleCheck = operatorLexeme + str[start];
	if (OPERATORS.find(operatorLexeme) == OPERATORS.end() && OPERATORS.find(doubleCheck) != OPERATORS.end())
	{
		operatorLexeme += str[start++];
	}

	tokens.push_back(Token(tokens.size() + 1, TokenType::OPERATOR, operatorLexeme, OPERATORS.at(operatorLexeme), lineNumber));
	return start;
}

size_t Lexer::processKeyword(std::string str, size_t lineNumber, size_t start)
{
	size_t len = str.length();
	std::string lexem(1, std::tolower(str[start++]));
	size_t id = tokens.size() + 1;
	while (start < len && (str[start] == '_' || std::isalnum(str[start])))
	{
		lexem += std::tolower(str[start++]);
	}
	if (KEYWORDS.find(lexem) != KEYWORDS.end())
	{
		tokens.push_back(Token(id, TokenType::KEYWORD, lexem, KEYWORDS.at(lexem), lineNumber));
		return start;
	}
	else if (isIdentificator(lexem, lineNumber))
	{
		tokens.push_back(Token(id, TokenType::IDENTIFIER, lexem, "Identifier", lineNumber));
		return start;
	}
	processError("Wrong identifier `" + lexem + "`", lineNumber);
	return len;
}

void Lexer::printErrors()
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
	std::cout << "\n=== LEXICAL ERRORS ===\n"
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
		processError("Unknown character `" + std::string(1, symb) + "`", lineNumber);
		break;
	}
}

std::vector<Token> Lexer::analyzeFile(const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Can't open the file: " << filename << "\n";
		return std::vector<Token>();
	}

	std::string line;
	size_t i = 0;
	while (getline(file, line))
	{
		analyzeLine(trim(line), i + 1);
		++i;
	}
	file.close();

	if (!errors.empty())
	{
		printErrors();
		return std::vector<Token>();
	}
	return tokens;
}
