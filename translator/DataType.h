#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <iostream>
#include <cstring>
#include <cstdint>
#include <variant>
#include "Constants.h"

using Type = std::variant<std::string, int64_t, uint64_t, long double>;

class DataType
{
private:
	int8_t toInt8t();
	int16_t toInt16t();
	int32_t toInt32t();
	uint8_t toUInt8t();
	uint16_t toUInt16t();
	uint32_t toUInt32t();
	float toFloat();
	double toDouble();
	void setCharValue(std::string);
	void setIntValue(long long);
	void setUIntValue(unsigned long long);
	void setRealValue(long double);
	void assign();
	void reAssign();
	void prepare(DataType *);
	void toUnsigned();
	void toSigned();

public:
	bool isArray = false;
	size_t len = 0;
	std::string type = "";
	size_t index = 0;
	int64_t v_int = 0;
	uint64_t v_uint = 0;
	long double v_real = 0.0;
	std::string v_char = "";

	int64_t *ptr_int = nullptr;
	uint64_t *ptr_uint = nullptr;
	long double *ptr_real = nullptr;

	DataType(std::string, size_t len = 0);
	DataType(DataType *);
	std::string toString(std::vector<std::string>);
	void setValue(Type);
	std::string getValue();
	std::string getFirst();
	void Add(DataType *, int mode = 1);
	void Mul(DataType *, bool isSigned);
	void Div(DataType *, bool isSigned);
	void Inc(int mode = 1);
	void Neg();
	void And(DataType *);
	void Or(DataType *);
	void Xor(DataType *);
	void Not();
	void lShift(DataType *, bool);
	void rShift(DataType *, bool);
	std::string cmp(DataType *);
};

#endif