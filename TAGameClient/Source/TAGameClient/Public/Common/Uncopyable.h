#pragma once


namespace ta
{
	// 복사 대입이 안되는 클래스 상속받아서 사용
	class Uncopyable
	{
	// 단독으로 사용될 수 없음 무조건 상속받아서 사용해야함 
	// 해당 클래스로 캐스팅해서 사용하면 안됨 속성부여만 해야함
	protected:
		Uncopyable(void) noexcept {}
		virtual ~Uncopyable(void) noexcept {}

	// 복사 대입을 막아줌
	private:
		Uncopyable(const Uncopyable&);
		Uncopyable& operator=(const Uncopyable&);
	};
}
