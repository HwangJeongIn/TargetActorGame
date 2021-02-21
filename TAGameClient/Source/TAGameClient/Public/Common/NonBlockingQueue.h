#pragma once

#include "Common/CommonBase.h"


//#define CAS_ONLY
namespace ta
{
	//typedef int T;

	class NonBlockingQueue;
	class NonBlockingQueueNode;
}


namespace ta
{
	class NonBlockingQueueNode
	{
	public:
		friend NonBlockingQueue;

		NonBlockingQueueNode(void) noexcept;
		explicit NonBlockingQueueNode(const int& value) noexcept;
		virtual ~NonBlockingQueueNode(void) noexcept;
		NonBlockingQueueNode& operator=(const NonBlockingQueueNode& other) noexcept;

	private:
		int _value;
		NonBlockingQueueNode* _next;
	};

}

namespace ta
{
	// 메모리 재사용으로 인한 ABA문제가 발생할 수 있다. 그래서 StampPtr을 사용한다.

	// 예를들어 스레드1에서 dequeue CAS 하려던 상황이고 // head = a이고, 그다음노드가 b인 상황
	// 다른스레드에서 a, b 노드를 삭제 // a와 b 삭제
	// 근데 다시 다른스레드에서 enqueue하다가 // a가 있던 메모리가 우연히 head에 다시 머무르게 되면
	// 스레드1에서 깨어났을때 CAS 성공(현재 a가 head이므로) 근데 b의 값은 어떨지 모른다. => ABA문제
	class StampPtr
	{
	public:
		StampPtr(void) noexcept;
		explicit StampPtr(NonBlockingQueueNode* ptr, int stamp) noexcept;

		void setPtr(NonBlockingQueueNode* node) noexcept;
		NonBlockingQueueNode* getPtr(void) noexcept;
		void setStamp(const int stamp) noexcept;
		int getStamp(void) const noexcept;

	public:
		// 16진수 16자리중 상위 4자리(16비트 = 2바이트) 사용(어차피 유저모드에서 사용안됨 여기 상위 16비트 활용하자)
		// 원래 변수 2가지 사용했는데 CAS사용하기 위해서 8바이트정도로 맞춰야했다 // 그래서 같은 변수로 사용하였다.
		//NonBlockingQueueNode* volatile _ptr;
		//volatile int _stamp;
		volatile unsigned long long _ptrAndStamp;
		//NonBlockingQueueNode* _debugValue;
	};

}


namespace ta
{
	class NonBlockingQueue
	{
	public:
		NonBlockingQueue(void) noexcept;
		virtual ~NonBlockingQueue(void) noexcept;

		void enqueue(const int& value) noexcept;
		bool dequeue(int& value) noexcept;

		void clear(void) noexcept;
		void print(void) noexcept;

	private:

		bool CAS(NonBlockingQueueNode* volatile* targetNode
				 , NonBlockingQueueNode* oldNode
				 , NonBlockingQueueNode* newNode) noexcept;

		bool STAMPCAS(StampPtr* address
					  , NonBlockingQueueNode* oldNode
					  , const int oldStamp
					  , NonBlockingQueueNode* newNode) noexcept;


	private:
		// 노드 자체가 아닌 가리키는 주소가 volatile
#ifndef CAS_ONLY
		StampPtr _head;
		StampPtr _tail;
#else
		NonBlockingQueueNode* volatile _head;
		NonBlockingQueueNode* volatile _tail;
#endif
	};
}