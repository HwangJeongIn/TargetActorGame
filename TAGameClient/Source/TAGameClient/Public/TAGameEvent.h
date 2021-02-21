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
	};

	TAGameEvent(const GameEventType gameEventType) noexcept;
	virtual ~TAGameEvent(void) noexcept;

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept = 0;

	GameEventType getGameEventType(void) const noexcept;

public:
	ta::ActorKey _actorKey;

private:
	GameEventType _gameEventType;
};


class TAGameEventSpawnActor : public TAGameEvent
{
public:
	TAGameEventSpawnActor(void) noexcept;
	virtual ~TAGameEventSpawnActor(void) noexcept;

public:

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept override final;
};


class TAGameEventDestroyActor : public TAGameEvent
{
public:
	TAGameEventDestroyActor(void) noexcept;
	virtual ~TAGameEventDestroyActor(void) noexcept;

public:

	virtual bool processEvent(TAGameEventProcessParameter& parameter) noexcept override final;
};

