#include "Server/ServerAiActorComponent.h"
#include "Common/ScopedLock.h"
#include "Common/AiPathPointPath.h"
#include "Common/GetComponentAndSystem.h"
#include "Server/ServerMoveActorSystem.h"


namespace ta
{
	ServerAiActorComponent::ServerAiActorComponent(void) noexcept
		: _currentAiState(AiState::Disabled)
		, _aiClassType(AiClassType::Count)
		, _aiPathPointPath(nullptr)
	{
	}

	ServerAiActorComponent::~ServerAiActorComponent(void) noexcept
	{
		if (nullptr != _aiPathPointPath)
		{
			delete _aiPathPointPath;
		}
	}

	bool ServerAiActorComponent::initializeData(const ComponentData* componentData) noexcept
	{
		if (false== CommonAiActorComponent::initializeData(componentData))
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		const CommonAiComponentData* data = static_cast<const CommonAiComponentData*>(componentData);
		{
			ScopedLock lock(this);
			if (false == setAiClass_(data->_aiClassType))
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}

			_aiPathPointPath = nullptr;
			if (true == data->_pathPointPathKey.isValid())
			{
				ServerMoveActorSystem* moveSystem = GetActorSystem<ServerMoveActorSystem>();
				const PathPointPath* rv = moveSystem->getPathPointPath(data->_pathPointPathKey);
				if (nullptr == rv)
				{
					TA_ASSERT_DEV(false, "비정상입니다.");
					return false;
				}
				
				_aiPathPointPath = new AiPathPointPath(*rv, getOwnerActorKey());
			}

		}

		return true;
	}

	const AiState& ServerAiActorComponent::getCurrentAiState_(void) const noexcept
	{
		return _currentAiState;
	}
	
	bool ServerAiActorComponent::setCurrentAiState_(const AiState& aiState) noexcept
	{
		if (AiState::Count == aiState)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		_currentAiState = aiState;
		return true;
	}
	
	const AiClassType& ServerAiActorComponent::getAiClassType_(void) const noexcept
	{
		return _aiClassType;
	}

	bool ServerAiActorComponent::setAiClass_(const AiClassType& aiClassType) noexcept
	{
		if (AiClassType::Count == aiClassType)
		{
			TA_ASSERT_DEV(false, "비정상입니다.");
			return false;
		}

		switch (aiClassType)
		{
		case AiClassType::Player:
			{
				//_ai.initializeAsPlayer_();
			}
			break;
		case AiClassType::Monster:
			{
				_ai.initializeAsMonster_();
			}
			break;
		case AiClassType::Npc:
			{
				_ai.initializeAsNpc_();
			}
			break;
		default:
			{
				TA_ASSERT_DEV(false, "비정상입니다.");
				return false;
			}
			break;

			TA_COMPILE_DEV(3 == static_cast<uint8>(AiClassType::Count), "여기도 추가해주세요");
		}

		_aiClassType = aiClassType;

		return true;
	}
	
	AiBehaviorTree& ServerAiActorComponent::getAi(void) noexcept
	{
		return _ai;
	}

	bool ServerAiActorComponent::hasPathPointPath_(void) const noexcept
	{
		return (nullptr != _aiPathPointPath);
	}
	
	const AiPathPointPath* ServerAiActorComponent::getAiPathPointPath_(void) const noexcept
	{
		return _aiPathPointPath;
	}

	AiPathPointPath* ServerAiActorComponent::getAiPathPointPath_(void) noexcept
	{
		return _aiPathPointPath;
	}
}
