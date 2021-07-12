#pragma once


#include "Common/CommonDataTypeDefinition.h"
#include <functional>


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


// 여기에 추가하세요															 
KEY_DECLARATION(ActorKey, uint32, 0)
KEY_DECLARATION(CharacterDBNo, int32, -1) // DB에 저장되는 값
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


#undef KEY_DECLARATION				
