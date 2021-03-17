#include "Common/Serializer.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include <string>


namespace ta
{
	MemoryBuffer::MemoryBuffer(void) noexcept
		: _data(nullptr)
		, _numBytes(0)
		, _maxBytes(0)
	{
		allocBuffer(1024 * 1024);
	}

	MemoryBuffer::~MemoryBuffer(void) noexcept
	{
		resetBuffer();
	}

	void MemoryBuffer::copyBuffer(char* input, int64 inputNum) noexcept
	{
		growBuffer(inputNum);
		memcpy(_data, input, _numBytes);

		_numBytes = inputNum;
		setBeginPos(); // _currentPos = _numBytes;
	}

	void MemoryBuffer::prepareCopyBuffer(int64 inputNum) noexcept
	{
		growBuffer(inputNum);
		_numBytes = inputNum;
		setBeginPos();
	}

	void MemoryBuffer::resetBuffer(void) noexcept
	{
		if (nullptr != _data)
		{
			free(_data);
			_data = nullptr;
		}

		_maxBytes = 0;
		_numBytes = 0;
		_currentPos = 0;
	}

	void MemoryBuffer::setBeginPos(void) noexcept
	{
		_currentPos = 0;
	}

	void MemoryBuffer::setEndPos(void) noexcept
	{
		_currentPos = _numBytes;
	}

	bool MemoryBuffer::write(void* input, int64 num, const TADataType& dataType) noexcept
	{
		const bool rv = write(input, _currentPos, num, dataType);
		if (true == rv)
		{
			_currentPos += num;
		}

		return rv;
	}

	bool MemoryBuffer::write(void* input, int64 offset, int64 num, const TADataType& dataType) noexcept
	{
		if ((nullptr == _data) || (offset < 0) || (num <= 0))
		{
			return false;
		}

		const int64 finalPos = offset + num;
		while (_maxBytes < finalPos)
		{
			growBuffer(_maxBytes * 2);
		}

		_numBytes = (finalPos < _numBytes) ? _numBytes : finalPos;
		memcpy(&_data[offset], input, num);

#ifdef CAN_CREATE_LOG_FILE
		writeLog(input, offset, num, dataType, true);
#endif

		return true;
	}

	bool MemoryBuffer::read(void* input, int64 num, const TADataType& dataType) noexcept
	{
		const bool rv = read(input, _currentPos, num, dataType);
		if (true == rv)
		{
			_currentPos += num;
		}

		return rv;
	}

	bool MemoryBuffer::read(void* input, int64 offset, int64 num, const TADataType& dataType) noexcept
	{
		if ((nullptr == _data) || (offset < 0) || (num <= 0))
		{
			return false;
		}

		const int64 finalPos = offset + num;
		if (_numBytes < finalPos)
		{
			TA_ASSERT_DEV(false, "read => buffer overrun");
			return false;
		}

		memcpy(input, &_data[offset], num);

#ifdef CAN_CREATE_LOG_FILE
		writeLog(input, offset, num, dataType, false);
#endif

		return true;
	}

#ifdef CAN_CREATE_LOG_FILE
	void MemoryBuffer::writeLog(void* input, int64 offset, int64 num, const TADataType& dataType, bool isWriteMode) noexcept
	{
		std::string log;
		if (true == isWriteMode)
		{
			log = "\n /Write => " ;
		}
		else
		{
			log = "\n /Read => " ;
		}
		
		std::string dataTypeString;
		ConvertEnumToString(dataType, dataTypeString);
		log += dataTypeString + " : ";

		switch (dataType)
		{
		case TADataType::Int8:
			{
				log += std::to_string(*(int8*)(input));
			}
			break;
		case TADataType::Int16:
			{
				log += std::to_string(*(int16*)(input));
			}
			break;
		case TADataType::Int32:
			{
				log += std::to_string(*(int32*)(input));
			}
			break;
		case TADataType::Int64:
			{
				log += std::to_string(*(int64*)(input));
			}
			break;
		case TADataType::Uint8:
			{
				log += std::to_string(*(uint8*)(input));
			}
			break;
		case TADataType::Uint16:
			{
				log += std::to_string(*(uint16*)(input));
			}
			break;
		case TADataType::Uint32:
			{
				log += std::to_string(*(uint32*)(input));
			}
			break;
		case TADataType::Uint64:
			{
				log += std::to_string(*(uint64*)(input));
			}
			break;
		case TADataType::Bool:
			{
				log += std::to_string(*(bool*)(input));
			}
			break;
		case TADataType::Float:
			{
				log += std::to_string(*(float*)(input));
			}
			break;
		default:
			{
				TA_COMPILE_DEV(10 == static_cast<uint8>(TADataType::Count), "여기도 확인해주세요");
				log += "unknown type";
			}
			break;
		}

		_logData += log;
	}
#endif

	int64 MemoryBuffer::getDataSize(void) const noexcept
	{
		return _numBytes;
	}

	char* MemoryBuffer::getData(void) noexcept
	{
		return _data;
	}

	const char* MemoryBuffer::getData(void) const noexcept
	{
		return _data;
	}

	bool MemoryBuffer::exportToFile(const fs::path& filePath) noexcept
	{
		
		std::string result;
		if (false == FileLoader::saveFileString(filePath, *this))
		{
			TA_ASSERT_DEV(false, "saveFileString failed");
			return false;
		}

		return true;
	}

	bool MemoryBuffer::importFromFile(const fs::path& filePath) noexcept
	{
		char* result = nullptr;
		uint32 resultSize = 0;
		if (false == FileLoader::loadFileString(filePath, *this))
		{
			TA_ASSERT_DEV(false, "importFromFile failed");
			return false;
		}

		return true;
	}

#ifdef CAN_CREATE_LOG_FILE
	bool MemoryBuffer::exportLogData(const fs::path& filePath) noexcept
	{
		fs::path logPath = filePath;
		logPath += ".log";

		if (false == FileLoader::saveFileString(logPath, _logData))
		{
			TA_ASSERT_DEV(false, "exportLogData failed");
			return false;
		}
		return true;
	}
#endif

	void MemoryBuffer::allocBuffer(const int64 size) noexcept
	{
		resetBuffer();

		_data = (char*)malloc(size);
		memset(_data, 0, size);
		_maxBytes = size;
		_numBytes = 0;
		_currentPos = 0;
	}

	void MemoryBuffer::growBuffer(const int64 size) noexcept
	{
		if (size <= _maxBytes)
		{
			TA_LOG_DEV("current size is bigger");
			return;
		}

		TA_LOG_DEV("grow buffer");
		char* temp = _data;
		_data = (char*)malloc(size);
		memset(_data, 0, size);
		memcpy(_data, temp, _maxBytes);
		_maxBytes = size;
		free(temp);
	}
}


namespace ta
{
	Serializer::Serializer(void) noexcept
		: _mode(SerializerMode::Read)
	{
	}

	Serializer::~Serializer(void) noexcept
	{
	}
	
	void Serializer::setMode(const SerializerMode input) noexcept
	{
		_mode = input;
	}

	const Serializer::SerializerMode Serializer::getMode(void) const noexcept
	{
		return _mode;
	}

	bool Serializer::exportToFile(const fs::path& filePath) noexcept
	{
		return _buffer.exportToFile(filePath);
	}

	bool Serializer::importFromFile(const fs::path& filePath) noexcept
	{
		return _buffer.importFromFile(filePath);
	}

	bool Serializer::exportLogData(const fs::path& filePath) noexcept
	{
		return _buffer.exportLogData(filePath);
	}

	Serializer& Serializer::operator<<(uint8& value) noexcept
	{
		serialize(&value, sizeof(uint8), TADataType::Uint8);
		return *this;
	}

	Serializer& Serializer::operator<<(uint16& value) noexcept
	{
		serialize(&value, sizeof(uint16), TADataType::Uint16);
		return *this;
	}

	Serializer& Serializer::operator<<(uint32& value) noexcept
	{
		serialize(&value, sizeof(uint32), TADataType::Uint32);
		return *this;
	}

	Serializer& Serializer::operator<<(uint64& value) noexcept
	{
		serialize(&value, sizeof(uint64), TADataType::Uint64);
		return *this;
	}

	Serializer& Serializer::operator<<(int8& value) noexcept
	{
		serialize(&value, sizeof(int8), TADataType::Int8);
		return *this;
	}

	Serializer& Serializer::operator<<(int16& value) noexcept
	{
		serialize(&value, sizeof(int16), TADataType::Int16);
		return *this;
	}

	Serializer& Serializer::operator<<(int32& value) noexcept
	{
		serialize(&value, sizeof(int32), TADataType::Int32);
		return *this;
	}

	Serializer& Serializer::operator<<(int64& value) noexcept
	{
		serialize(&value, sizeof(int64), TADataType::Int64);
		return *this;
	}

	Serializer& Serializer::operator<<(bool& value) noexcept
	{
		serialize(&value, sizeof(bool), TADataType::Bool);
		return *this;
	}

	Serializer& Serializer::operator<<(float& value) noexcept
	{
		serialize(&value, sizeof(float), TADataType::Float);
		return *this;
	}

	bool Serializer::serialize(void* data, int64 num, const TADataType& dataType) noexcept
	{
		if (SerializerMode::Read == _mode)
		{
			return _buffer.read(data, num, dataType);
		}
		else
		{
			return _buffer.write(data, num, dataType);
		}
	}
}

