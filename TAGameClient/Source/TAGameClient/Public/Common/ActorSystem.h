#pragma once

#include "Common/CommonBase.h"
#include "Common/Uncopyable.h"

namespace ta
{
	class ActorSystemUpdateParameter;
	class ActorComponent;
}


namespace ta
{
	class ActorSystem : public Uncopyable
	{
	public:
		virtual void update(const ActorSystemUpdateParameter& updateParameter) const noexcept;
		const ActorSystemType getActorSystemType(void) const noexcept;

		virtual bool initialize(void) noexcept;
		virtual bool open(void) noexcept;
		virtual void close(void) noexcept;
		
		virtual ~ActorSystem(void) noexcept;
	protected:
		// 자체로 생성 못함
		explicit ActorSystem(const ActorSystemType& actorSystemType) noexcept;


	private:
		ActorSystemType _actorSystemType;
	};
}


namespace ta
{
	class ActorSystemUpdateParameter
	{
	public:
		ActorSystemUpdateParameter(void) noexcept;
		~ActorSystemUpdateParameter(void) noexcept;

		void reset(void) noexcept;

	public:

		float _deltaTime;

		// 컴포넌트(데이터) 관련
		ActorComponent* _components;
		uint32 _componentCount;
	};
}

