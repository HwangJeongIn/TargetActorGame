#pragma once
#include "Common/CommonBase.h"
#include <vector>


namespace ta
{
	class MemoryStreamBase
	{
	public:
		MemoryStreamBase(void) noexcept;
		virtual ~MemoryStreamBase(void) noexcept;

		const uint64 getBufferLength(void) const noexcept;
		const char* getBufferPtr(void) const noexcept;

	protected:
		void clear(void) noexcept;

	protected:
		char* _buffer;
		uint64 _head;
		uint64 _capacity;
	};
}


namespace ta
{
	class OutputMemoryStream : public MemoryStreamBase
	{
	public:
		OutputMemoryStream(void) noexcept;
		virtual ~OutputMemoryStream(void) noexcept;

		void write(const void* data, const uint64 size) noexcept;

		// 공유되지않는 데이터만 // 포인터로 참조된다면 안됨
		template<typename T>
		void write(const std::vector<T>& dataList) noexcept
		{
			// vector내부의 여러 포인터값을 무턱대고 직렬화해버리면 다른 호스트에서 쓰레기값이 읽힐 수 있다
			// count를 직렬화하고 원소를 하나씩 직렬화하자

			const uint32 listCount = dataList.size();
			write<uint32>(listCount);
			for (uint32 index = 0; index < listCount; ++index)
			{
				write<T>(dataList[index]);
			}
		}

		template<typename T> 
		void write(const T& data) noexcept
		{
			const bool isPrimitiveDataType = (std::is_arithmetic<T>::value || std::is_enum<T>::value);
			if (false == isPrimitiveDataType)
			{
				// 
			}
			else
			{
				write(&data, sizeof(data));
			}
		}

	private:
		bool reallocateBuffer(const uint64 newLength) noexcept;
	};
}


namespace ta
{
	class InputMemoryStream : public MemoryStreamBase
	{
	public:
		InputMemoryStream(char* targetBuffer, const uint64 length) noexcept;
		virtual ~InputMemoryStream(void) noexcept;

		const uint64 getRemainingDataSize(void) const noexcept;

		void read(void* outputData, const uint64 size) noexcept;

		// 공유되지않는 데이터만 // 포인터로 참조된다면 안됨
		template<typename T>
		void read(std::vector<T>& outputDataList) noexcept
		{
			// count를 읽고, 원소를 하나씩 읽어오자

			uint32 listCount = 0;
			read<uint32>(listCount);
			outputDataList.resize(listCount);
			for (uint32 index = 0; index < listCount; ++index)
			{
				read<T>(outputDataList[index]);
			}
		}

		template<typename T> 
		void read(T& outputData) noexcept
		{
			const bool isPrimitiveDataType = (std::is_arithmetic<T>::value || std::is_enum<T>::value);
			if (false == isPrimitiveDataType)
			{
				// 
			}
			else
			{
				read(&outputData, sizeof(outputData));
			}
		}
	};
}

