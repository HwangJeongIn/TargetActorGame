#pragma once

#include "Common/CommonBase.h"
#include <filesystem>

namespace fs = std::filesystem;

#define CAN_CREATE_LOG_FILE

namespace ta
{
	class MemoryBuffer
	{
	public:
		MemoryBuffer(void) noexcept;
		virtual ~MemoryBuffer(void) noexcept;

		void copyBuffer(char* input, int64 inputNum) noexcept;	// 복사할 버퍼가 실체가 있는 경우
		void prepareCopyBuffer(int64 inputNum) noexcept;		// 복사할 버퍼가 실체가 없는 경우, 데이터 복사는 알아서 해줘야한다. ex) 파일에서 그대로 읽어올 때
		void resetBuffer(void) noexcept;
		void setBeginPos(void) noexcept;
		void setEndPos(void) noexcept;

		bool write(void* input, int64 num, const TADataType& dataType) noexcept;
		bool write(void* input, int64 offset, int64 num, const TADataType& dataType) noexcept;

		bool read(void* input, int64 num, const TADataType& dataType) noexcept;
		bool read(void* input, int64 offset, int64 num, const TADataType& dataType) noexcept;

#ifdef CAN_CREATE_LOG_FILE
		void writeLog(void* input, int64 offset, int64 num, const TADataType& dataType, bool isWriteMode) noexcept;
#endif

		int64 getDataSize(void) const noexcept;
		char* getData(void) noexcept;
		const char* getData(void) const noexcept;

		bool exportToFile(const fs::path& filePath) noexcept;
		bool importFromFile(const fs::path& filePath) noexcept;

#ifdef CAN_CREATE_LOG_FILE
		bool exportLogData(const fs::path& filePath) noexcept;
#endif

	private:
		void allocBuffer(const int64 size) noexcept;
		void growBuffer(const int64 size) noexcept;

	private:
		int64 _currentPos;

		char* _data;
		int64 _numBytes;
		int64 _maxBytes;

#ifdef CAN_CREATE_LOG_FILE
		std::string _logData;
#endif
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
		const Serializer::SerializerMode getMode(void) const noexcept;
		
		bool exportToFile(const fs::path& filePath) noexcept;
		bool importFromFile(const fs::path& filePath) noexcept;

#ifdef CAN_CREATE_LOG_FILE
		bool exportLogData(const fs::path& filePath) noexcept;
#endif

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
		bool serialize(void* data, int64 num, const TADataType& dataType) noexcept;

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
