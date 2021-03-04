#pragma once

#include "Common/CommonBase.h"
#include "Common/KeyDefinition.h"


class UTAGameInstance;
class UWorld;
class TAGameEvent;


extern bool RegisterTAGameEvent(TAGameEvent* gameEvent) noexcept;


class TAGameEventProcessParameter
{
public:
	TAGameEventProcessParameter(void) noexcept;
	~TAGameEventProcessParameter(void) noexcept;

public:
	UTAGameInstance* _gameInstance;
	UWorld* _world;
};


class TAGameEvent
{
public:
	enum class GameEventType : uint8
	{
		None = 0
		, SpawnActor
		, DestroyActor
		, InitializeInventory
	};

	explicit TAGameEvent(const GameEventType gameEventType) noexcept;
	virtual ~TAGameEvent(void) noexcept;

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept = 0;

	GameEventType getGameEventType(void) const noexcept;
	const ta::ActorKey getActorKey(void) const noexcept;
	bool setActorKey(const ta::ActorKey& actorKey) noexcept;

private:
	GameEventType _gameEventType;
	ta::ActorKey _actorKey;
};


class TAGameEventSpawnActor : public TAGameEvent
{
public:
	TAGameEventSpawnActor(void) noexcept;
	virtual ~TAGameEventSpawnActor(void) noexcept;

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept override final;
};


class TAGameEventDestroyActor : public TAGameEvent
{
public:
	TAGameEventDestroyActor(void) noexcept;
	virtual ~TAGameEventDestroyActor(void) noexcept;

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept override final;
};


class TAGameEventInitializeInventory : public TAGameEvent
{
public:
	TAGameEventInitializeInventory(void) noexcept;
	virtual ~TAGameEventInitializeInventory(void) noexcept;

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept override final;

	bool setCapacity(const ta::int32 capacity) noexcept;

private:
	ta::int32 _capacity;
};

