#include "Common/Serializer.h"
#include "Common/FileLoader.h"
#include "Common/EnumUtility.h"
#include "Common/StringUtility.h"
#include <string>


namespace ta
{
	MemoryBuffer::MemoryBuffer(Serializer* owner /*= nullptr*/) noexcept
		: _owner(owner)
		, _data(nullptr)
		, _numBytes(0)
		, _maxBytes(0)
	{
		// 그냥	버퍼만 필요한경우가 있을듯 // 파일 스트링만 읽어서 파싱할 때 굳이 Serializer가 필요없다
		//TA_ASSERT_DEV((nullptr != _owner), "owner must not be nullptr!")
		allocBuffer(1024 * 1024);
	}

	MemoryBuffer::~MemoryBuffer(void) noexcept
	{
		resetBuffer();
	}

	void MemoryBuffer::setOwner(Serializer* owner) noexcept
	{
		_owner = owner;
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

	bool MemoryBuffer::write(const void* input, int64 num, const TADataType& dataType) noexcept
	{
		const bool rv = write(input, _currentPos, num, dataType);
		if (true == rv)
		{
			_currentPos += num;
		}

		return rv;
	}

	bool MemoryBuffer::write(const void* input, int64 offset, int64 num, const TADataType& dataType) noexcept
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
	void MemoryBuffer::writeLog(const void* input, int64 offset, int64 num, const TADataType& dataType, bool isWriteMode) noexcept
	{
		if (nullptr == _owner || false == _owner->hasModeFlag(SerializerMode::WriteLog))
		{
			return;
		}

		std::string log = "\n(" + ToStringCast<int64>(_currentPos) + ")";
		if (true == isWriteMode)
		{
			log += "/Write => ";
		}
		else
		{
			log += "/Read => " ;
		}
		
		std::string dataTypeString;
		ConvertEnumToString(dataType, dataTypeString);
		log += dataTypeString + " : ";

		switch (dataType)
		{
		case TADataType::Int8:
			{
				// 로그 잘보이게하기 위해서
				log += ToStringCast<int16>(*(const int8*)(input));
			}
			break;
		case TADataType::Int16:
			{
				log += ToStringCast<int16>(*(const int16*)(input));
			}
			break;
		case TADataType::Int32:
			{
				log += ToStringCast<int32>(*(const int32*)(input));
			}
			break;
		case TADataType::Int64:
			{
				log += ToStringCast<int64>(*(const int64*)(input));
			}
			break;
		case TADataType::Uint8:
			{
				// 로그 잘보이게하기 위해서
				log += ToStringCast<uint16>(*(const uint8*)(input));
			}
			break;
		case TADataType::Uint16:
			{
				log += ToStringCast<uint16>(*(const uint16*)(input));
			}
			break;
		case TADataType::Uint32:
			{
				log += ToStringCast<uint32>(*(const uint32*)(input));
			}
			break;
		case TADataType::Uint64:
			{
				log += ToStringCast<uint64>(*(const uint64*)(input));
			}
			break;
		case TADataType::Bool:
			{
				log += ToStringCast<bool>(*(const bool*)(input));
			}
			break;
		case TADataType::Float:
			{
				log += ToStringCast<float>(*(const float*)(input));
			}
			break;
		case TADataType::String:
		case TADataType::StdString:
			{
				log += (const char*)(input);
			}
			break;
		default:
			{
				TA_COMPILE_DEV(12 == static_cast<uint8>(TADataType::Count), "여기도 확인해주세요");
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

	bool MemoryBuffer::exportBinaryToFile(const fs::path& filePath) noexcept
	{
		if (false == FileLoader::saveBinaryFile(filePath, *this))
		{
			TA_ASSERT_DEV(false, "saveFileString failed");
			return false;
		}

		return true;
	}

	bool MemoryBuffer::importBinaryFromFile(const fs::path& filePath) noexcept
	{
		if (false == FileLoader::loadBinaryFile(filePath, *this))
		{
			TA_ASSERT_DEV(false, "importStringFromFile failed");
			return false;
		}

		return true;
	}

	bool MemoryBuffer::exportStringToFile(const fs::path& filePath) noexcept
	{
		if (false == FileLoader::saveFileString(filePath, *this))
		{
			TA_ASSERT_DEV(false, "saveFileString failed");
			return false;
		}

		return true;
	}

	bool MemoryBuffer::importStringFromFile(const fs::path& filePath) noexcept
	{
		if (false == FileLoader::loadFileString(filePath, *this))
		{
			TA_ASSERT_DEV(false, "importStringFromFile failed");
			return false;
		}

		return true;
	}

#ifdef CAN_CREATE_LOG_FILE
	bool MemoryBuffer::exportLogData(const fs::path& filePath) noexcept
	{
		fs::path logPath = filePath;
		logPath += ".log";

		std::string sizeString("\nFileSize : ");
		sizeString += ToStringCast<int64>(_numBytes);
		
		if (false == FileLoader::saveFileString(logPath, _logData + sizeString))
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
			//TA_LOG_DEV("current size is bigger");
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
	Serializer::Serializer(MemoryBuffer* buffer) noexcept
		: _modeFlag(0)
		, _buffer(buffer)
		, _mustBeDeleted(false)
	{
		if (nullptr == _buffer)
		{
			_buffer = new MemoryBuffer(this);
			_mustBeDeleted = true;
		}
		else
		{
			_buffer->setOwner(this);
		}
	}

	Serializer::~Serializer(void) noexcept
	{
		if (true == _mustBeDeleted)
		{
			delete _buffer;
		}
	}

	MemoryBuffer& Serializer::getBuffer(void) noexcept
	{
		return *_buffer;
	}

	const MemoryBuffer& Serializer::getBuffer(void) const noexcept
	{
		return *_buffer;
	}

	char* Serializer::getRawBuffer(int64& numBytes) noexcept
	{
		numBytes = _buffer->getDataSize();
		return _buffer->getData();
	}

	const char* Serializer::getRawBuffer(int64& numBytes) const noexcept
	{
		numBytes = _buffer->getDataSize();
		return _buffer->getData();
	}

	void Serializer::setModeFlag(const SerializerModeFlag input) noexcept
	{
		if ((SerializerMode::Read & input) && (SerializerMode::Write & input))
		{
			TA_ASSERT_DEV(false, "read and write mode is not supported.");
			return;
		}
		_modeFlag = input;
	}

	void Serializer::turnOnModeFlag(const SerializerModeFlag input) noexcept
	{
		_modeFlag |= input;
	}

	void Serializer::turnOffModeFlag(const SerializerModeFlag input) noexcept
	{
		_modeFlag &= ~input;
	}

	void Serializer::toggleModeFlag(const SerializerModeFlag input) noexcept
	{
		_modeFlag ^= input;
	}

	bool Serializer::hasModeFlag(const SerializerModeFlag input) const noexcept
	{
		return 0 != (input & _modeFlag);
	}

	const SerializerModeFlag Serializer::getModeFlag(void) const noexcept
	{
		return _modeFlag;
	}

	bool Serializer::exportBinaryToFile(const fs::path& filePath) noexcept
	{
		return _buffer->exportBinaryToFile(filePath);
	}

	bool Serializer::importBinaryFromFile(const fs::path& filePath) noexcept
	{
		return _buffer->importBinaryFromFile(filePath);
	}

	bool Serializer::exportStringToFile(const fs::path& filePath) noexcept
	{
		return _buffer->exportStringToFile(filePath);
	}

	bool Serializer::importStringFromFile(const fs::path& filePath) noexcept
	{
		return _buffer->importStringFromFile(filePath);
	}

#ifdef CAN_CREATE_LOG_FILE
	bool Serializer::exportLogData(const fs::path& filePath) noexcept
	{
		return _buffer->exportLogData(filePath);
	}
#endif

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

	Serializer& Serializer::operator<<(char* value) noexcept
	{
		serialize(value, strlen(value), TADataType::String);
		return *this;
	}

	Serializer& Serializer::operator<<(std::string& value) noexcept
	{
		serialize(value.data(), value.size(), TADataType::StdString);
		return *this;
	}

	bool Serializer::serialize(void* data, int64 num, const TADataType& dataType) noexcept
	{
		if (true == hasModeFlag(SerializerMode::Read))
		{
			return _buffer->read(data, num, dataType);
		}
		else if (true == hasModeFlag(SerializerMode::Write))
		{
			return _buffer->write(data, num, dataType);
		}

		TA_ASSERT_DEV(false, "read or write mode isn't set.");
		return false;
	}
}

