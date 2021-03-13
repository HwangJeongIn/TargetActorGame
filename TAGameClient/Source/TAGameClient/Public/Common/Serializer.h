#pragma once

#include "Common/CommonBase.h"


namespace ta
{
	class MemoryBuffer
	{
	public:
		MemoryBuffer(void) noexcept;
		virtual ~MemoryBuffer(void) noexcept;

		void allocBuffer(const int64 size) noexcept;
		void resetBuffer(void) noexcept;

		void setBeginPos(void) noexcept;

		bool write(void* input, int64 num) noexcept;
		bool write(void* input, int64 offset, int64 num) noexcept;

		bool read(void* input, int64 num) noexcept;
		bool read(void* input, int64 offset, int64 num) noexcept;

		int64 getDataSize(void) const noexcept;
		uint8* getData(void) noexcept;

		bool exportToFile(const std::string& filePath) noexcept;
		bool importFromFile(const std::string& filePath) noexcept;

	private:
		int64 _currentPos;

		uint8* _data;
		int64 _numBytes;
		int64 _maxBytes;
	};

}


namespace ta
{
	class Serializer
	{
	public:
		enum class SerializerMode : uint8
		{
			Read = 0
			, Write
		};

		Serializer(void) noexcept;
		virtual ~Serializer(void) noexcept;

		void setMode(const SerializerMode input) noexcept;
		void allocBuffer(const int64 size) noexcept;
		
		bool exportToFile(const std::string& filePath) noexcept;
		bool importFromFile(const std::string& filePath) noexcept;

		Serializer& operator<<(uint8& value) noexcept;
		Serializer& operator<<(uint16& value) noexcept;
		Serializer& operator<<(uint32& value) noexcept;
		Serializer& operator<<(uint64& value) noexcept;
		Serializer& operator<<(int8& value) noexcept;
		Serializer& operator<<(int16& value) noexcept;
		Serializer& operator<<(int32& value) noexcept;
		Serializer& operator<<(int64& value) noexcept;
		Serializer& operator<<(bool& value) noexcept;
		Serializer& operator<<(float& value) noexcept;

	private:
		bool serialize(void* data, int64 num) noexcept;

		//template<typename T>
		//Serializer& serialize(T& Value)
		//{
		//	static_assert(!TIsSigned<T>::Value, "To reduce the number of template instances, cast 'Value' to a uint16&, uint32& or uint64& prior to the call or use ByteOrderSerialize(void*, int32).");
		//
		//	if (!IsByteSwapping()) // Most likely case (hot path)
		//	{
		//		Serialize(&Value, sizeof(T));
		//		return *this;
		//	}
		//	return SerializeByteOrderSwapped(Value); // Slowest and unlikely path (but fastest than SerializeByteOrderSwapped(void*, int32)).
		//}

	private:
		MemoryBuffer _buffer;
		SerializerMode _mode;

	};

}
