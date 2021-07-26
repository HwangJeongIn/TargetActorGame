#pragma once


#include "Common/CommonDataTypeDefinition.h"
//#include <string>


//#define Keyname ActorKey
//#define KeyType uint32
//#define UndefinedKeyValueInput 0



#define KEY_DECLARATION(KeyName, KeyType, UndefinedKeyValueInput)  				 				\
namespace ta																	 				\
{																				 				\
typedef KeyType KeyName##Type;																	\
	class KeyName																 				\
	{																			 				\
		friend std::hash<KeyName>;												 				\
	public:																		 				\
		KeyName(void) noexcept;													 				\
		KeyName(const KeyType& keyValue) noexcept;								 				\
		KeyName(const KeyName& key) noexcept;									 				\
		KeyName& operator=(const KeyName& key) noexcept;						 				\
		KeyName& operator=(const KeyType& keyValue) noexcept;					 				\
		bool operator==(const ta::KeyName& key) const noexcept;									\
		bool operator!=(const ta::KeyName& key) const noexcept;									\
		const KeyType& getKeyValue(void) const noexcept;						 				\
		void setKeyValue(const KeyType& keyValue) noexcept;						 				\
		void clear(void) noexcept;												 				\
		bool isValid(void) const noexcept;										 				\
																								\
		static KeyType UndefinedKeyValue;										 				\
	private:																	 				\
		KeyType _keyValue;														 				\
	};																			 				\
																								\
}																				 				\
																				 				\
namespace std																	 				\
{																				 				\
	template <>																	 				\
	class hash<ta::KeyName>														 				\
	{																			 				\
	public:																		 				\
		size_t operator()(const ta::KeyName& key) const							 				\
		{																		 				\
			return key.getKeyValue();											 				\
		}																		 				\
	};																			 				\
}																				 				\


namespace ta
{
	// 새로운 키가 추가되었을때 다른곳에서도 변경되어야 할 코드를 인지시키기 위해 추가
	enum class KeyType : uint8
	{
		ActorKey = 0
		, CharacterDBNoKey
		, SectorKey
		, PathPointPathKey
		, SpawnDataGroupKey
		, GroupGameDataKey
		, MoveGameDataKey
		, AiGameDataKey
		, CharacterGameDataKey
		, ItemGameDataKey
		, RenderingGameDataKey
		, ConditionGameDataKey
		, EventGameDataKey
		, SectorZoneGameDataKey
		, BuffGameDataKey
		, Count
	};
}

namespace ta
{
	TA_COMPILE_DEV(15 == static_cast<uint8>(KeyType::Count), "여기도 추가하세요");
}

// 여기에 추가하세요															 
KEY_DECLARATION(ActorKey, uint32, 0)
KEY_DECLARATION(CharacterDBNoKey, int32, -1) // DB에 저장되는 값
KEY_DECLARATION(SectorKey, int32, -1)
KEY_DECLARATION(PathPointPathKey, hash_value, 0) // 파일이름 해시값
KEY_DECLARATION(SpawnDataGroupKey, hash_value, 0) // 파일이름 해시값


// GameData Key
KEY_DECLARATION(GroupGameDataKey, int32, -1)
KEY_DECLARATION(MoveGameDataKey, int32, -1)
KEY_DECLARATION(AiGameDataKey, int32, -1)
KEY_DECLARATION(CharacterGameDataKey, int32, -1)
KEY_DECLARATION(ItemGameDataKey, int32, -1)
KEY_DECLARATION(RenderingGameDataKey, int32, -1)
KEY_DECLARATION(ConditionGameDataKey, int32, -1)
KEY_DECLARATION(EventGameDataKey, int32, -1)
KEY_DECLARATION(SectorZoneGameDataKey, int32, -1)
KEY_DECLARATION(BuffGameDataKey, int32, -1)


#undef KEY_DECLARATION				
