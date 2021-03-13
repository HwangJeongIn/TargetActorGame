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

	void MemoryBuffer::allocBuffer(const int64 size) noexcept
	{
		resetBuffer();

		_data = (uint8*)malloc(size);
		memset(_data, 0, size);
		_maxBytes = size;
		_numBytes = 0;
		_currentPos = 0;
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
		if (_maxBytes < finalPos)
		{
			TA_ASSERT_DEV(false, "write => buffer overrun");
			return false;
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

	uint8* MemoryBuffer::getData(void) noexcept
	{
		return _data;
	}

	bool MemoryBuffer::exportToFile(const std::string& filePath) noexcept
	{
		
		std::string result;
		//memcpy(result[0], )
		if (false == FileLoader::saveFileString(filePath, (char*)_data))
		{
			TA_ASSERT_DEV(false, "read => buffer overrun");
			return false;
		}

		return true;
	}

	bool MemoryBuffer::importFromFile(const std::string& filePath) noexcept
	{
		std::string result;
		if (false == FileLoader::loadFileString(filePath, result))
		{
			TA_ASSERT_DEV(false, "read => buffer overrun");
			return false;
		}

		return true;
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

	void Serializer::allocBuffer(const int64 size) noexcept
	{
		_buffer.allocBuffer(size);
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

