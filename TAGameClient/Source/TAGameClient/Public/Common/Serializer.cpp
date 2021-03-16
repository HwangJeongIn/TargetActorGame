#include "Common/Serializer.h"
#include "Common/FileLoader.h"
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

	bool MemoryBuffer::write(void* input, int64 num) noexcept
	{
		const bool rv = write(input, _currentPos, num);
		if (true == rv)
		{
			_currentPos += num;
		}

		return rv;
	}

	bool MemoryBuffer::write(void* input, int64 offset, int64 num) noexcept
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

		return true;
	}

	bool MemoryBuffer::read(void* input, int64 num) noexcept
	{
		const bool rv = read(input, _currentPos, num);
		if (true == rv)
		{
			_currentPos += num;
		}

		return rv;
	}

	bool MemoryBuffer::read(void* input, int64 offset, int64 num) noexcept
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
		return true;
	}

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

	bool MemoryBuffer::exportToFile(const std::string& filePath) noexcept
	{
		
		std::string result;
		if (false == FileLoader::saveFileString(filePath, *this))
		{
			TA_ASSERT_DEV(false, "saveFileString failed");
			return false;
		}

		return true;
	}

	bool MemoryBuffer::importFromFile(const std::string& filePath) noexcept
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

	bool Serializer::exportToFile(const std::string& filePath) noexcept
	{
		return _buffer.exportToFile(filePath);
	}

	bool Serializer::importFromFile(const std::string& filePath) noexcept
	{
		return _buffer.importFromFile(filePath);
	}

	Serializer& Serializer::operator<<(uint8& value) noexcept
	{
		serialize(&value, sizeof(uint8));
		return *this;
	}

	Serializer& Serializer::operator<<(uint16& value) noexcept
	{
		serialize(&value, sizeof(uint16));
		return *this;
	}

	Serializer& Serializer::operator<<(uint32& value) noexcept
	{
		serialize(&value, sizeof(uint32));
		return *this;
	}

	Serializer& Serializer::operator<<(uint64& value) noexcept
	{
		serialize(&value, sizeof(uint64));
		return *this;
	}

	Serializer& Serializer::operator<<(int8& value) noexcept
	{
		serialize(&value, sizeof(int8));
		return *this;
	}

	Serializer& Serializer::operator<<(int16& value) noexcept
	{
		serialize(&value, sizeof(int16));
		return *this;
	}

	Serializer& Serializer::operator<<(int32& value) noexcept
	{
		serialize(&value, sizeof(int32));
		return *this;
	}

	Serializer& Serializer::operator<<(int64& value) noexcept
	{
		serialize(&value, sizeof(int64));
		return *this;
	}

	Serializer& Serializer::operator<<(bool& value) noexcept
	{
		serialize(&value, sizeof(bool));
		return *this;
	}

	Serializer& Serializer::operator<<(float& value) noexcept
	{
		serialize(&value, sizeof(float));
		return *this;
	}

	bool Serializer::serialize(void* data, int64 num) noexcept
	{
		if (SerializerMode::Read == _mode)
		{
			return _buffer.read(data, num);
		}
		else
		{
			return _buffer.write(data, num);
		}
	}
}

