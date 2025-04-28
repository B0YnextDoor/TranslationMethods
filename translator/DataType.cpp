#include "DataType.h"

int8_t DataType::toInt8t()
{
	return static_cast<int8_t>(v_int);
}

int16_t DataType::toInt16t()
{
	return static_cast<int16_t>(v_int);
}

int32_t DataType::toInt32t()
{
	return static_cast<int32_t>(v_int);
}

uint8_t DataType::toUInt8t()
{
	return static_cast<uint8_t>(v_uint);
}

uint16_t DataType::toUInt16t()
{
	return static_cast<uint16_t>(v_uint);
}

uint32_t DataType::toUInt32t()
{
	return static_cast<uint32_t>(v_int);
}

float DataType::toFloat()
{
	return static_cast<float>(v_real);
}

double DataType::toDouble()
{
	return static_cast<double>(v_real);
}

void DataType::setCharValue(std::string value)
{
	v_char += value;
}

void DataType::setIntValue(long long value)
{
	v_int = value;
	if (isArray && index < len)
		ptr_int[index++] = v_int;
	if (index == len)
		index = 0;
}

void DataType::setUIntValue(unsigned long long value)
{
	v_uint = value;
	if (isArray && index < len)
		ptr_uint[index++] = v_uint;
	if (index == len)
		index = 0;
}

void DataType::setRealValue(long double value)
{
	v_real = value;
	if (isArray && index < len)
		ptr_real[index++] = v_real;
	if (index == len)
		index = 0;
}

void DataType::assign()
{
	if (!isArray)
		return;
	if (type[0] == 'i' && ptr_int)
	{
		if (type == "int8_t" || type == "int8_t*")
			v_int = static_cast<int64_t>(static_cast<int8_t>(*ptr_int));
		else if (type == "int16_t" || type == "int16_t*")
			v_int = static_cast<int64_t>(static_cast<int16_t>(*ptr_int));
		else if (type == "int32_t" || type == "int32_t*")
			v_int = static_cast<int64_t>(static_cast<int32_t>(*ptr_int));
		else
			v_int = static_cast<int64_t>(*ptr_int);
	}
	else if (type[0] == 'u' && ptr_uint)
	{
		if (type == "uint8_t" || type == "uint8_t*")
			v_uint = static_cast<uint64_t>(static_cast<int8_t>(*ptr_uint));
		else if (type == "uint16_t" || type == "uint16_t*")
			v_uint = static_cast<uint64_t>(static_cast<uint16_t>(*ptr_uint));
		else if (type == "uint32_t" || type == "uint32_t*")
			v_uint = static_cast<uint64_t>(static_cast<uint32_t>(*ptr_uint));
		else
			v_uint = static_cast<uint64_t>(*ptr_uint);
	}
	else if (type[0] == 'c' && v_char.length() > 0)
		v_uint = v_char[0];
	else if (ptr_real)
	{
		if (type == "float" || type == "float*")
			v_real = static_cast<long double>(static_cast<float>(*ptr_real));
		else if (type == "double" || type == "double*")
			v_real = static_cast<long double>(static_cast<double>(*ptr_real));
		else
			v_real = static_cast<long double>(*ptr_real);
	}
}

void DataType::reAssign()
{
	if (!isArray)
		return;
	if (type[0] == 'i' && ptr_int)
	{
		if (type == "int8_t" || type == "int8_t*")
			ptr_int[0] = static_cast<int64_t>(static_cast<int8_t>(v_int));
		else if (type == "int16_t" || type == "int16_t*")
			ptr_int[0] = static_cast<int64_t>(static_cast<int16_t>(v_int));
		else if (type == "int32_t" || type == "int32_t*")
			ptr_int[0] = static_cast<int64_t>(static_cast<int32_t>(v_int));
		else
			ptr_int[0] = static_cast<int64_t>(v_int);
	}
	else if (type[0] == 'u' && ptr_uint)
	{
		if (type == "uint8_t" || type == "uint8_t*")
			ptr_uint[0] = static_cast<uint64_t>(static_cast<int8_t>(v_uint));
		else if (type == "uint16_t" || type == "uint16_t*")
			ptr_uint[0] = static_cast<uint64_t>(static_cast<uint16_t>(v_uint));
		else if (type == "uint32_t" || type == "uint32_t*")
			ptr_uint[0] = static_cast<uint64_t>(static_cast<uint32_t>(v_uint));
		else
			ptr_uint[0] = static_cast<uint64_t>(v_uint);
	}
	else if (type[0] == 'c' && v_char.length() > 0)
		v_char[0] = char(v_uint);
	else if (ptr_real)
	{
		if (type == "float" || type == "float*")
			ptr_real[0] = static_cast<long double>(static_cast<float>(v_real));
		else if (type == "double" || type == "double*")
			ptr_real[0] = static_cast<long double>(static_cast<double>(v_real));
		else
			ptr_real[0] = static_cast<long double>(v_real);
	}
}

void DataType::prepare(DataType *data)
{
	assign();
	if (data)
		data->assign();
}

void DataType::toUnsigned()
{
	if (type[0] == 'u' || type[0] == 'c')
		return;
	int typeSize;
	if (isArray && (ptr_int || ptr_real))
		ptr_uint = new uint64_t[len];
	if (type[0] == 'i')
	{
		typeSize = INT_TYPES.find(type)->second;
		type = "u" + type;
		if (typeSize == INT8_T)
		{
			v_uint = static_cast<uint64_t>(static_cast<uint8_t>(v_int));
			if (isArray && ptr_int)
				for (size_t i = 0; i < len; ++i)
					ptr_uint[i] = static_cast<uint64_t>(static_cast<uint8_t>(ptr_int[i]));
		}
		else if (typeSize == INT16_T)
		{
			v_uint = static_cast<uint64_t>(static_cast<uint16_t>(v_int));
			if (isArray && ptr_int)
				for (size_t i = 0; i < len; ++i)
					ptr_uint[i] = static_cast<uint64_t>(static_cast<uint16_t>(ptr_int[i]));
		}
		else if (typeSize == INT32_T)
		{
			v_uint = static_cast<uint64_t>(static_cast<uint32_t>(v_int));
			if (isArray && ptr_int)
				for (size_t i = 0; i < len; ++i)
					ptr_uint[i] = static_cast<uint64_t>(static_cast<uint32_t>(ptr_int[i]));
		}
		else
		{
			v_uint = static_cast<uint64_t>(v_int);
			if (isArray && ptr_int)
				for (size_t i = 0; i < len; ++i)
					ptr_uint[i] = static_cast<uint64_t>(ptr_int[i]);
		}
	}
	else
	{
		typeSize = REAL_TYPES.find(type)->second;
		if (typeSize == FLOAT)
		{
			v_uint = static_cast<uint64_t>(static_cast<uint32_t>(v_real));
			type = "uint32_t";
			if (isArray && ptr_real)
				for (size_t i = 0; i < len; ++i)
					ptr_uint[i] = static_cast<uint64_t>(static_cast<uint32_t>(ptr_real[i]));
		}
		else
		{
			type = "uint64_t";
			v_uint = static_cast<uint64_t>(v_real);
			if (isArray && ptr_real)
				for (size_t i = 0; i < len; ++i)
					ptr_uint[i] = static_cast<uint64_t>(ptr_real[i]);
		}
	}
	if (isArray)
		type += "*";
}

void DataType::toSigned()
{
	if (type[0] != 'u' && type[0] != 'c')
		return;
	if (type[0] == 'c')
		type = "uint8_t";
	if (isArray && ptr_uint)
		ptr_int = new int64_t[len];
	int typeSize = INT_TYPES.find(type)->second;
	if (typeSize == INT8_T)
	{
		v_int = static_cast<int64_t>(static_cast<int8_t>(v_uint));
		type = "int8_t";
		if (isArray && ptr_uint)
			for (size_t i = 0; i < len; ++i)
				ptr_int[i] = static_cast<int64_t>(static_cast<int8_t>(ptr_uint[i]));
	}
	else if (typeSize == INT16_T)
	{
		v_int = static_cast<int64_t>(static_cast<int16_t>(v_uint));
		type = "int16_t";
		if (isArray && ptr_uint)
			for (size_t i = 0; i < len; ++i)
				ptr_int[i] = static_cast<int64_t>(static_cast<int16_t>(ptr_uint[i]));
	}
	else if (typeSize == INT32_T)
	{
		v_int = static_cast<int64_t>(static_cast<int32_t>(v_uint));
		type = "int32_t";
		if (isArray && ptr_uint)
			for (size_t i = 0; i < len; ++i)
				ptr_int[i] = static_cast<int64_t>(static_cast<int32_t>(ptr_uint[i]));
	}
	else
	{
		v_uint = static_cast<uint64_t>(v_int);
		type = "int64_t";
		if (isArray && ptr_uint)
			for (size_t i = 0; i < len; ++i)
				ptr_int[i] = static_cast<int64_t>(ptr_uint[i]);
	}
	if (isArray)
		type += "*";
}

DataType::DataType(std::string type, size_t len) : type(type), len(len)
{
	isArray = len != 0;
	if (!isArray)
		return;
	if (type[0] == 'i')
		ptr_int = new int64_t[len];
	else if (type[0] == 'u')
		ptr_uint = new uint64_t[len];
	else if (type[0] != 'c')
		ptr_real = new long double[len];
}

DataType::DataType(DataType *data)
{
	isArray = data->isArray;
	len = data->len;
	type = data->type;
	v_int = data->v_int;
	v_uint = data->v_uint;
	v_real = data->v_real;
	v_char = data->v_char;
	ptr_int = data->ptr_int;
	ptr_uint = data->ptr_uint;
	ptr_real = data->ptr_real;
}

std::string DataType::toString(std::vector<std::string> info)
{
	std::string res = type + " ";
	for (const auto &str : info)
	{
		res += str + " ";
	}
	return res + getValue();
}

void DataType::setValue(Type value)
{
	if (type[0] == 'i')
		return setIntValue(std::get<int64_t>(value));
	else if (type[0] == 'u')
		return setUIntValue(std::get<uint64_t>(value));
	else if (type[0] != 'c')
		return setRealValue(std::get<long double>(value));
	return setCharValue(std::get<std::string>(value));
}

std::string DataType::getValue()
{
	if (type[0] == 'c')
		return v_char;
	if (!isArray)
	{
		if (type[0] == 'i')
			return std::to_string(v_int);
		else if (type[0] == 'u')
			return std::to_string(v_uint);
		return std::to_string(v_real);
	}
	std::string result = "";
	if (type[0] == 'i' && ptr_int)
	{
		for (size_t i = 0; i < len; ++i)
			result += std::to_string(*(ptr_int + i)) + " ";
	}
	else if (type[0] == 'u' && ptr_uint)
	{
		for (size_t i = 0; i < len; ++i)
			result += std::to_string(*(ptr_uint + i)) + " ";
	}
	else
	{
		for (size_t i = 0; i < len; ++i)
			result += std::to_string(*(ptr_real + i)) + " ";
	}
	return result;
}

std::string DataType::getFirst()
{
	std::string res = getValue();
	if (!isArray)
		return res;
	if (!index)
		return "";
	size_t count = 0, i = 0;
	while (i < res.size())
	{
		if (res[i++] != ' ')
			continue;
		++count;
		if (count == index)
			break;
	}
	return res.substr(i, res.find(' ', i) - i);
}

void DataType::Add(DataType *data, int mode)
{
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() + static_cast<int8_t>(data->toInt8t() * mode)));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() + static_cast<int16_t>(data->toInt16t() * mode)));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() + static_cast<int32_t>(data->toInt32t() * mode)));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int + static_cast<int64_t>(data->v_int * mode));
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
	{
		auto val1 = toUInt8t(), val2 = data->toUInt8t();
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(mode > 0 ? val1 + val2 : val1 - val2));
	}
	else if (type == "uint16_t" || type == "uint16_t*")
	{
		auto val1 = toUInt16t(), val2 = data->toUInt16t();
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(mode > 0 ? val1 + val2 : val1 - val2));
	}
	else if (type == "uint32_t" || type == "uint32_t*")
	{
		auto val1 = toUInt32t(), val2 = data->toUInt32t();
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(mode > 0 ? val1 + val2 : val1 - val2));
	}
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(mode > 0 ? v_uint + data->v_uint : v_uint - data->v_uint);
	else if (type == "float" || type == "float*")
		v_real = static_cast<long double>(static_cast<float>(toFloat() + data->toFloat() * mode));
	else if (type == "double" || type == "double*")
		v_real = static_cast<long double>(static_cast<double>(toDouble() + data->toDouble() * mode));
	else if (type == "long double" || type == "long double*")
		v_real = static_cast<long double>(v_real + data->v_real * mode);
	reAssign();
}

void DataType::Mul(DataType *data, bool isSigned)
{
	if (!isSigned)
	{
		toUnsigned();
		data->toUnsigned();
	}
	else
	{
		toSigned();
		data->toSigned();
	}
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() * data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() * data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() * data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int * data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() * data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() * data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() * data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint * data->v_uint);
	else if (type == "float" || type == "float*")
		v_real = static_cast<long double>(static_cast<float>(toFloat() * data->toFloat()));
	else if (type == "double" || type == "double*")
		v_real = static_cast<long double>(static_cast<double>(toDouble() * data->toDouble()));
	else if (type == "long double" || type == "long double*")
		v_real = static_cast<long double>(v_real * data->v_real);
	reAssign();
}

void DataType::Div(DataType *data, bool isSigned)
{
	if (!isSigned)
	{
		toUnsigned();
		data->toUnsigned();
	}
	else
	{
		toSigned();
		data->toSigned();
	}
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() / data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() / data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() / data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int / data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() / data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() / data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() / data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint / data->v_uint);
	else if (type == "float" || type == "float*")
		v_real = static_cast<long double>(static_cast<float>(toFloat() / data->toFloat()));
	else if (type == "double" || type == "double*")
		v_real = static_cast<long double>(static_cast<double>(toDouble() / data->toDouble()));
	else if (type == "long double" || type == "long double*")
		v_real = static_cast<long double>(v_real / data->v_real);
	reAssign();
}

void DataType::Inc(int mode)
{
	if (isArray)
	{
		++index;
		if (index == len)
			index = 0;
	}
	else if (type == "int8_t")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() + mode));
	else if (type == "int16_t")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() + mode));
	else if (type == "int32_t")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() + mode));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int + mode);
	else if (type == "uint8_t")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() + mode));
	else if (type == "uint16_t")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() + mode));
	else if (type == "uint32_t")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() + mode));
	else if (type == "uint64_t")
		v_uint = static_cast<uint64_t>(v_uint + mode);
	else if (type == "float")
		v_real = static_cast<long double>(static_cast<float>(toFloat() + mode));
	else if (type == "double")
		v_real = static_cast<long double>(static_cast<double>(toDouble() + mode));
	else if (type == "long double")
		v_real = static_cast<long double>(v_real + mode);
}

void DataType::Neg()
{
	prepare(nullptr);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() * -1));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() * -1));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() * -1));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int * -1);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() * -1));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() * -1));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() * -1));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint * -1);
	else if (type == "float" || type == "float*")
		v_real = static_cast<long double>(static_cast<float>(toFloat() * -1));
	else if (type == "double" || type == "double*")
		v_real = static_cast<long double>(static_cast<double>(toDouble() * -1));
	else if (type == "long double" || type == "long double*")
		v_real = static_cast<long double>(v_real * -1);
	reAssign();
}

void DataType::And(DataType *data)
{
	prepare(data);
	if (type[0] != 'c' && type.find("int") == std::string::npos)
	{
		toUnsigned();
		data->toUnsigned();
	}
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() & data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() & data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() & data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int & data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() & data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() & data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() & data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint & data->v_uint);
	reAssign();
}

void DataType::Or(DataType *data)
{
	prepare(data);
	if (type[0] != 'c' && type.find("int") == std::string::npos)
	{
		toUnsigned();
		data->toUnsigned();
	}
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() | data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() | data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() | data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int | data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() | data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() | data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() | data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint | data->v_uint);
	reAssign();
}

void DataType::Xor(DataType *data)
{
	if (type[0] != 'c' && type.find("int") == std::string::npos)
	{
		toUnsigned();
		data->toUnsigned();
	}
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() ^ data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() ^ data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() ^ data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int ^ data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() ^ data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() ^ data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() ^ data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint ^ data->v_uint);
	reAssign();
}

void DataType::Not()
{
	if (type[0] != 'c' && type.find("int") == std::string::npos)
		toUnsigned();
	prepare(nullptr);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(~toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(~toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(~toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(~v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(~toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(~toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(~toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(~v_uint);
	reAssign();
}

void DataType::lShift(DataType *data, bool isSigned)
{
	if (!isSigned)
	{
		toUnsigned();
		data->toUnsigned();
	}
	else
	{
		toSigned();
		data->toSigned();
	}
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() << data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() << data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() << data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int << data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() << data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() << data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() << data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint << data->v_uint);
	reAssign();
}

void DataType::rShift(DataType *data, bool isSigned)
{
	if (!isSigned)
	{
		toUnsigned();
		data->toUnsigned();
	}
	else
	{
		toSigned();
		data->toSigned();
	}
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
		v_int = static_cast<int64_t>(static_cast<int8_t>(toInt8t() >> data->toInt8t()));
	else if (type == "int16_t" || type == "int16_t*")
		v_int = static_cast<int64_t>(static_cast<int16_t>(toInt16t() >> data->toInt16t()));
	else if (type == "int32_t" || type == "int32_t*")
		v_int = static_cast<int64_t>(static_cast<int32_t>(toInt32t() >> data->toInt32t()));
	else if (type == "int64_t" || type == "int64_t*")
		v_int = static_cast<int64_t>(v_int >> data->v_int);
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
		v_uint = static_cast<uint64_t>(static_cast<uint8_t>(toUInt8t() >> data->toUInt8t()));
	else if (type == "uint16_t" || type == "uint16_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint16_t>(toUInt16t() >> data->toUInt16t()));
	else if (type == "uint32_t" || type == "uint32_t*")
		v_uint = static_cast<uint64_t>(static_cast<uint32_t>(toUInt32t() >> data->toUInt32t()));
	else if (type == "uint64_t" || type == "uint64_t*")
		v_uint = static_cast<uint64_t>(v_uint >> data->v_uint);
	reAssign();
}

std::string DataType::cmp(DataType *data)
{
	prepare(data);
	if (type == "int8_t" || type == "int8_t*")
	{
		int8_t res = static_cast<int8_t>(toInt8t() - data->toInt8t());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "int16_t" || type == "int16_t*")
	{
		int16_t res = static_cast<int16_t>(toInt16t() - data->toInt16t());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "int32_t" || type == "int32_t*")
	{
		int32_t res = static_cast<int32_t>(toInt32t() - data->toInt32t());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "int64_t" || type == "int64_t*")
	{
		int64_t res = static_cast<int64_t>(v_int - data->v_int);
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "uint8_t" || type == "uint8_t*" || type == "char*")
	{
		uint8_t res = static_cast<uint8_t>(toUInt8t() - data->toUInt8t());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "uint16_t" || type == "uint16_t*")
	{
		uint16_t res = static_cast<uint16_t>(toUInt16t() - data->toUInt16t());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "uint32_t" || type == "uint32_t*")
	{
		uint32_t res = static_cast<uint32_t>(toUInt32t() - data->toUInt32t());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "uint64_t" || type == "uint64_t*")
	{
		uint64_t res = static_cast<uint64_t>(v_uint - data->v_uint);
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "float" || type == "float*")
	{
		float res = static_cast<float>(toFloat() - data->toFloat());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	else if (type == "double" || type == "double*")
	{
		double res = static_cast<double>(toDouble() - data->toDouble());
		return res < 0 ? "<" : res > 0 ? ">"
									   : "==";
	}
	long double res = static_cast<long double>(v_real - data->v_real);
	return res < 0 ? "<" : res > 0 ? ">"
								   : "==";
}
