#include <unordered_map>
#include <vector>
#include <string>
#include <regex>

const std::regex IDENTIFIER_REGEX = std::regex("^[a-zA-Z_.][a-zA-Z0-9_]*$");

const std::regex BIN_REGEX = std::regex("^[01]+[bB]$|^0[bB][01]+$");
const std::regex OCT_REGEX = std::regex("^[0-7]+[oOqQ]$|^0[qQoO][0-7]+$");
const std::regex HEX_REGEX = std::regex("^0[xX][0-9A-Fa-f]+$|^[0-9A-Fa-f]+[hH]$");
const std::regex DEC_REGEX = std::regex("^[0-9]+(\\.[0-9]*)?[dD]?$|^[0-9]+[eE][-+]?[0-9]+$");

const std::unordered_map<std::string, std::string> OPERATORS = {{"+", "Addition opertor"}, {"-", "Subtraction operator"}, {"*", "Multiplication"}, {"/", "Unsigned division"}, {"//", "Signed division"}, {"|", "Bitwise OR"}, {"^", "Bitwise XOR"}, {"&", "Bitwise AND"}, {"<<", "Left shift"}, {">>", "Right shift"}};

const std::unordered_map<std::string, std::string> SEPARATORS = {
	{",", "Comma"}, {":", "Colon"}, {"(", "Left parentheses"}, {")", "Right parentheses"}, {"[", "Left square bracket"}, {"]", "Right square bracket"}};

const std::unordered_map<std::string, std::string> KEYWORDS = {{"section", "Defines code section"}, {"global", "Defines export to other modules"}, {".data", "Data section"}, {".rodata", "Immutable data section"}, {".bss", "Section keep uninitialized & global variables"}, {".text", "Program code section"}, {"rax", "64 bit accumulator register"}, {"eax", "32 bit accumulator register"}, {"ax", "16 bit accumulator register"}, {"ah", "8 bit accumulator register"}, {"al", "8 bit accumulator register"}, {"rbx", "64 bit base register"}, {"ebx", "32 bit base register"}, {"bx", "16 bit base register"}, {"bh", "8 bit base register"}, {"bl", "8 bit base register"}, {"rxc", "64 bit counter register"}, {"ecx", "32 bit counter register"}, {"cx", "16 bit counter register"}, {"ch", "8 bit counter register"}, {"cl", "8 bit counter register"}, {"rdx", "64 bit data register"}, {"edx", "32 bit data register"}, {"dx", "16 bit data register"}, {"dh", "8 bit data register"}, {"dl", "8 bit data register"}, {"rsp", "64 bit stack pointer register"}, {"esp", "32 bit stack pointer register"}, {"sp", "16 bit stack pointer register"}, {"rbp", "64 bit base pointer register"}, {"ebp", "32 bit base pointer register"}, {"bp", "16 bit base pointer register"}, {"rsi", "64 bit sourse index register"}, {"esi", "32 bit sourse index register"}, {"si", "16 bit source index register"}, {"rdi", "64 bit destination index register"}, {"edi", "32 bit destination index register"}, {"di", "16 bit destination index register"}, {"r8", "64 bit register"}, {"r9", "64 bit register"}, {"r10", "64 bit register"}, {"r11", "64 bit register"}, {"r12", "64 bit register"}, {"r13", "64 bit register"}, {"r14", "64 bit register"}, {"r15", "64 bit register"}, {"db", "Defines 1 Byte"}, {"dw", "Defines 2 Bytes"}, {"dd", "Defines 4 Bytes"}, {"dq", "Defines 8 Bytes"}, {"dt", "Defines 10 Bytes"}, {"add", "Sum two numbers"}, {"sub", "Subtract one number from another"}, {"mul", "Multiplicate two numbers (without a sign)"}, {"imul", "Multiplicate two numbers (with a sign)"}, {"idiv", "Divide two numbers (with a sign)"}, {"div", "Divide two numbers (without a sign)"}, {"inc", "Increments the number"}, {"dec", "Decrements the number"}, {"neg", "Changes number's sign"}, {"and", "Bitwise AND"}, {"or", "Bitwise OR"}, {"xor", "Bitwise XOR"}, {"not", "Bitwise NOT"}, {"shl", "Bitwise SHL"}, {"sal", "Bitwise SAL"}, {"shr", "Bitwise SHR"}, {"sar", "Bitwise SAR"}, {"cmp", "Comares two values"}, {"jmp", "Unconditional Transfer"}, {"call", "Calls procedure"}, {"ret", "Returns from the procedure"}, {"loop", "Returns to label"}, {"je", "Transfer if equal"}, {"jne", "Transfer if not equal"}, {"ja", "Transfer if greater"}, {"jae", "Transfer if greater or equal"}, {"jb", "Transfer if less"}, {"jbe", "Transfer if less or equal"}, {"jg", "Transfer if greater (with a sign)"}, {"jge", "Transfer if greater or equal (with a sign)"}, {"jl", "Transfer if less (with a sign)"}, {"jle", "Transfer if less or equal (with a sign)"}, {"mov", "Moves the value"}, {"lea", "Loads effective address"}, {"push", "Put value in stack"}, {"pop", "Remove value from stack"}, {"equ", "Defines constant"}, {"int", "Calls interuption"}, {"extern", "Includes components from other modules"}, {"printf", "Print macro"}, {"byte", "Explicit byte type macro"}};

const std::vector<std::string> SECTION_NODE = {"section", ".data", ".bss", ".text", ".rodata"};
const std::string TEXT = ".text";

const std::string COLON = ":";
const std::string COMMA = ",";
const std::string RET = "ret";
const std::string CALL = "call";
const std::string EXTERN = "extern";
const std::string LBRACES = "([";
const std::string RBRACES = ")]";

const std::vector<std::string> INITIALIZATION_NODE = {"db", "dw", "dd", "dq", "dt", "equ"};
const std::vector<std::string> STREAM_INSTRUCTIONS = {"call", "loop", "ret", "int"};