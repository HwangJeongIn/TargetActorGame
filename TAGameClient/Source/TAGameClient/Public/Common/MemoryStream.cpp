#include "Common/MemoryStream.h"


namespace ta
{
	MemoryStreamBase::MemoryStreamBase(void) noexcept
	{
		clear();
	}

	MemoryStreamBase::~MemoryStreamBase(void) noexcept
	{
	}

	const uint64 MemoryStreamBase::getBufferLength(void) const noexcept
	{
		return _head;
	}

	const char* MemoryStreamBase::getBufferPtr(void) const noexcept
	{
		return _buffer;
	}

	void MemoryStreamBase::clear(void) noexcept
	{
		_buffer = nullptr;
		_head = 0;
		_capacity = 0;
	}
}

namespace ta
{
	OutputMemoryStream::OutputMemoryStream(void) noexcept
	{
		reallocateBuffer(32); // nullptr일때 하면 malloc이랑 동일
	}

	OutputMemoryStream::~OutputMemoryStream(void) noexcept
	{
		std::free(_buffer);
	}

	void OutputMemoryStream::write(const void* data, const uint64 size) noexcept
	{
		const uint64 resultHead = _head + size;
		if (_capacity < resultHead)
		{
			//if (false == reallocateBuffer(std::max(resultHead, _capacity * 2)))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return;
			}
		}

		std::memcpy(_buffer + _head, data, size);
		_head = resultHead;
	}

	bool OutputMemoryStream::reallocateBuffer(const uint64 newLength) noexcept
	{
		_buffer = static_cast<char*>(std::realloc(_buffer, newLength));
		if (nullptr == _buffer)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		_capacity = newLength;

		return true;
	}
}


namespace ta
{
	InputMemoryStream::InputMemoryStream(char* targetBuffer, const uint64 length) noexcept
	{
		_buffer = targetBuffer;
		_head = 0;
		_capacity = length;
	}

	InputMemoryStream::~InputMemoryStream(void) noexcept
	{
		//std::free(_buffer);
	}
	
	const uint64 InputMemoryStream::getRemainingDataSize(void) const noexcept
	{
		return _capacity - _head;
	}

	void InputMemoryStream::read(void* outputData, const uint64 size) noexcept
	{
		const uint64 resultHead = _head - size;
		if (_capacity < resultHead)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return;
		}

		std::memcpy(outputData, _buffer + _head, size);
		_head = resultHead;
	}
}
