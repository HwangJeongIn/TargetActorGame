#pragma once

#include "Common/CommonMoveActorSystem.h"



namespace ta
{
	class Vector;
	class CommonActor;
}


namespace ta
{
	class ClientMoveActorSystem : public CommonMoveActorSystem
	{
	public:
		ClientMoveActorSystem(void) noexcept;
		virtual ~ClientMoveActorSystem(void) noexcept;

		virtual bool initialize(void) noexcept override final;
		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;

		virtual bool processMoveActor(CommonActor* target
										 , const Vector& newPos
										 , const bool isForced) const noexcept override final;

		virtual bool processMoveToTarget(CommonMoveActorComponent* myMove, CommonMoveActorComponent* targetMove) const noexcept override final;
		virtual bool processMoveToRandomPoint(CommonMoveActorComponent* myMove) const noexcept override final;

		bool requestMoveActor(CommonActor* targetActor, const Vector& newPosition) const noexcept;
		bool requestMoveToTarget(CommonActor* myActor, CommonActor* targetActor) const noexcept;
		bool requestMoveToRandomPoint(CommonActor* myActor) const noexcept;


		bool respondMoveActor(CommonActor* target
							  , const Vector& newPos) const noexcept;

		bool respondRotateActor(CommonMoveActorComponent* myMove
							   , const Vector& newDirection) const noexcept;

		// Client에서 스폰완료후 Client Move Com의 onActive에서 호출
		bool setStartTransform(CommonActor* target
							   , const Vector& newPos
							   , const Vector& newRot) const noexcept;
	};
}
