#include "Common/KeyDefinition.h"




#define KEY_DEFINITION(KeyName, KeyType, UndefinedKeyValueInput)  				\
namespace ta																	\
{																				\
	KeyName::KeyName(void) noexcept : _keyValue(KeyName::UndefinedKeyValue) {}	\
	KeyName::KeyName(const KeyType& keyValue) noexcept : _keyValue(keyValue) {}	\
	KeyName::KeyName(const KeyName& key) noexcept : _keyValue(key._keyValue) {}	\
																				\
	KeyName& KeyName::operator=(const KeyName& key)	noexcept					\
	{																			\
		_keyValue = key._keyValue;												\
		return *this;															\
	}																			\
																				\
	KeyName& KeyName::operator=(const KeyType& keyValue) noexcept				\
	{																			\
		_keyValue = keyValue;													\
		return *this;															\
	}																			\
																				\
	bool KeyName::operator==(const ta::KeyName& key) const noexcept				\
	{																			\
		return (_keyValue == key._keyValue);									\
	}																			\
																				\
	bool KeyName::operator!=(const ta::KeyName& key) const noexcept				\
	{																			\
		return (_keyValue != key._keyValue);									\
	}																			\
																				\
	const KeyType& KeyName::getKeyValue(void) const noexcept					\
	{																			\
		return _keyValue;														\
	}																			\
																				\
	void KeyName::setKeyValue(const KeyType& keyValue) noexcept					\
	{																			\
		_keyValue = keyValue;													\
	}																			\
																				\
	void KeyName::clear(void) noexcept											\
	{																			\
		_keyValue = KeyName::UndefinedKeyValue;									\
	}																			\
																				\
	bool KeyName::isValid(void) const noexcept									\
	{																			\
		return (_keyValue != KeyName::UndefinedKeyValue);						\
	}																			\
																				\
	KeyType KeyName::UndefinedKeyValue = UndefinedKeyValueInput;				\
}																				\
																				\


// operator==은 같은 네임스페이스에 있어야한다.


// 여기에 추가하세요
KEY_DEFINITION(ActorKey, uint32, 0)
KEY_DEFINITION(CharacterDBNo, int32, -1) // DB에 저장되는 값
KEY_DEFINITION(SectorKey, int32, -1)
KEY_DEFINITION(PathPointPathKey, hash_value, 0)
KEY_DEFINITION(SpawnDataGroupKey, hash_value, 0) // 파일이름 해시값

// GameData Key
KEY_DEFINITION(GroupGameDataKey, int32, -1)
KEY_DEFINITION(MoveGameDataKey, int32, -1)
KEY_DEFINITION(AiGameDataKey, int32, -1)
KEY_DEFINITION(CharacterGameDataKey, int32, -1)
KEY_DEFINITION(ItemGameDataKey, int32, -1)
KEY_DEFINITION(RenderingGameDataKey, int32, -1)
KEY_DEFINITION(ConditionGameDataKey, int32, -1)
KEY_DEFINITION(SectorEventGameDataKey, int32, -1)
KEY_DEFINITION(SectorZoneGameDataKey, int32, -1)
																		 
#undef KEY_DEFINITION
