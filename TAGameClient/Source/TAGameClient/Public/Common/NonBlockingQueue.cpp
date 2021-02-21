#include "Common/NonBlockingQueue.h"
#include <atomic>


namespace ta
{
	NonBlockingQueueNode::NonBlockingQueueNode(void) noexcept
		: _value(0)
		, _next(nullptr)
	{
	}

	NonBlockingQueueNode::NonBlockingQueueNode(const int& value) noexcept
		: _value(value)
		, _next(nullptr)
	{
	}

	NonBlockingQueueNode::~NonBlockingQueueNode(void) noexcept
	{
	}

	NonBlockingQueueNode& NonBlockingQueueNode::operator=(const NonBlockingQueueNode& other) noexcept
	{
		_next = other._next;
		_value = other._value;
		return *this;
	}
}


namespace ta
{
	StampPtr::StampPtr(void) noexcept
		: _ptrAndStamp(0)
		//, _debugValue(nullptr)
	{
	}

	StampPtr::StampPtr(NonBlockingQueueNode* ptr, int stamp) noexcept
		: _ptrAndStamp(reinterpret_cast<unsigned long long>(ptr))
		//, _debugValue(ptr)
	{
		const unsigned long long stampValue = ((unsigned long long)stamp << (12 * 4));
		_ptrAndStamp |= stampValue;
	}

	void StampPtr::setPtr(NonBlockingQueueNode* node) noexcept
	{
		_ptrAndStamp = (reinterpret_cast<unsigned long long>(node));
		//_ptrAndStamp &= 0xFFFF0000'00000000; // ptr clear
		//_ptrAndStamp |= value; // set ptr
		//_debugValue = node;
	}

	NonBlockingQueueNode* StampPtr::getPtr(void) noexcept
	{
		return reinterpret_cast<NonBlockingQueueNode*>(_ptrAndStamp & 0x0000FFFF'FFFFFFFF);
	}

	void StampPtr::setStamp(const int stamp) noexcept
	{
		const unsigned long long stampValue = ((unsigned long long)stamp << (12 * 4));
		_ptrAndStamp &= 0x0000FFFF'FFFFFFFF; // stamp clear
		_ptrAndStamp |= stampValue; // set stamp
	}

	int StampPtr::getStamp(void) const noexcept
	{
		return (int)(_ptrAndStamp >> (12 * 4));
	}
}


namespace ta
{
	NonBlockingQueue::NonBlockingQueue(void) noexcept
	{
#ifndef CAS_ONLY
		NonBlockingQueueNode* dummy = new NonBlockingQueueNode(int());
		_head.setPtr(dummy); _tail.setPtr(dummy);
		////////////////////////////////////////////////////////////////////////
		//_ptr = _tail._ptr = new NonBlockingQueueNode(int());
#else
		_head = _tail = new NonBlockingQueueNode(int());
#endif
	}

	NonBlockingQueue::~NonBlockingQueue(void) noexcept
	{
	}

#define DEBUG_TEST
	void NonBlockingQueue::enqueue(const int& value) noexcept
	{
		// Tail이 가리키는 node에 CAS로 새노드 추가
		// 실패하면 Next로 이동한 후 추가
		// 성공하면 Tail을 이동

		// CAS(&(lastNode->_next), nullptr, newNode);
		// CAS(&_tail, lastNode, newNode);
		// thread1에서 첫번째 CAS성공만 했을경우 다른 thread2에서 첫번째 CAS는 무조건 실패하게 된다.
		// 이러면 블로킹 알고리즘이다. 
		// 그래서 thread1에서 첫번째 CAS만 성공한 상태이고 현재 thread2에서 enqueue해야하는 상황이라면
		// 두번째 CAS를 thread2에서 시도해주자 그리고 다시 돌자
		//cout << "enq : " << value << "\n";
		NonBlockingQueueNode* newNode = new NonBlockingQueueNode(value);
		while (true)
		{
#ifndef CAS_ONLY
			StampPtr lastNode = _tail;
			NonBlockingQueueNode* lastNodePtr = lastNode.getPtr();
			int lastNodeStamp = lastNode.getStamp();
			NonBlockingQueueNode* lastNextNode = lastNodePtr->_next;

#ifdef DEBUG_TEST
			// test
			StampPtr firstNode = _head;
			NonBlockingQueueNode* firstNodePtr = firstNode.getPtr();
			int firstNodeStamp = firstNode.getStamp();
			NonBlockingQueueNode* firstNextNode = firstNodePtr->_next;
#endif

			// 이상태는 첫번째와 두번째 CAS가 완료되어 다시 읽어야하는 상황이다.
			// lastNode에 tail을 받아왔지만 이 짧은 구간에 다른 스레드에서 들어와서 CAS를 모두 완료한것이다. 
			// 다시해준다.
			if (lastNodePtr != _tail.getPtr())
			{
				continue;
			}

			if (nullptr == lastNextNode)
			{
				if (true == CAS(&(lastNodePtr->_next), nullptr, newNode))
				{
					// 만약에 tail값은 미리 받아 놓은값이랑 같은데, stamp값이 다르다면 
					// 다른곳에서 삭제되었다가 해당 메모리가 다시 tail변수로 삽입된것이다.
					// stamp를 비교하고 아니면 실패하게된다.
					STAMPCAS(&_tail, lastNodePtr, lastNodeStamp, newNode);
#ifdef DEBUG_TEST
					{
						NonBlockingQueueNode* test1 = _tail.getPtr();
						NonBlockingQueueNode* test2 = _head.getPtr();
						int a = 3;
					}
#endif

					return;
				}
			}
			else
			{
				// 다른스레드 대신 전진시켜주는 CAS // 이렇게해서 블로킹되는 요소가 없어졌다.
				STAMPCAS(&_tail, lastNodePtr, lastNodeStamp, lastNextNode);
#ifdef DEBUG_TEST
				{
					NonBlockingQueueNode* test1 = _tail.getPtr();
					NonBlockingQueueNode* test2 = _head.getPtr();
					int a = 3;
				}
#endif
			}



			////////////////////////////////////////////////////////////////////////
			//StampPtr lastNode = _tail;
			//NonBlockingQueueNode* nextNode = lastNode._ptr->_next;

			//// 이상태는 첫번째와 두번째 CAS가 완료되어 다시 읽어야하는 상황이다.
			//// lastNode에 tail을 받아왔지만 이 짧은 구간에 다른 스레드에서 들어와서 CAS를 모두 완료한것이다. 
			//// 다시해준다.
			//if (lastNode._ptr != _tail._ptr)
			//{
			//	continue;
			//}

			//if (nullptr == nextNode)
			//{
			//	if (true == CAS(&(lastNode._ptr->_next), nullptr, newNode))
			//	{
			//		// 만약에 tail값은 미리 받아 놓은값이랑 같은데, stamp값이 다르다면 
			//		// 다른곳에서 삭제되었다가 해당 메모리가 다시 tail변수로 삽입된것이다.
			//		// stamp를 비교하고 아니면 실패하게된다.
			//		STAMPCAS(&_tail, lastNode._ptr, lastNode._stamp, newNode);
			//		return;
			//	}
			//}
			//else
			//{
			//	// 다른스레드 대신 전진시켜주는 CAS // 이렇게해서 블로킹되는 요소가 없어졌다.
			//	STAMPCAS(&_tail, lastNode._ptr, lastNode._stamp, nextNode);
			//}
#else
			NonBlockingQueueNode* lastNode = _tail;
			NonBlockingQueueNode* lastNextNode = lastNode->_next;

			// 이상태는 첫번째와 두번째 CAS가 완료되어 다시 읽어야하는 상황이다.
			// lastNode에 tail을 받아왔지만 이 짧은 구간에 다른 스레드에서 들어와서 CAS를 모두 완료한것이다. 
			// 다시해준다.
			if (lastNode != _tail)
			{
				continue;
			}

			if (nullptr != lastNextNode)
			{
				CAS(&_tail, lastNode, lastNextNode);
				continue;
			}

			if (false == CAS(&(lastNode->_next), nullptr, newNode))
			{
				continue;
			}
			CAS(&_tail, lastNode, newNode);
			return;

			//if (nullptr == lastNextNode)
			//{
			//	if (true == CAS(&(lastNode->_next), nullptr, newNode))
			//	{
			//		CAS(&_tail, lastNode, newNode);
			//		return;
			//	}
			//}
			//else
			//{ 
			//	// 다른스레드 대신 전진시켜주는 CAS // 이렇게해서 블로킹되는 요소가 없어졌다.
			//	CAS(&_tail, lastNode, lastNextNode);
			//}
#endif 
		}

	}

	bool NonBlockingQueue::dequeue(int& value) noexcept
	{
		while (true)
		{
#ifndef CAS_ONLY
			StampPtr firstNode = _head;
			NonBlockingQueueNode* firstNodePtr = firstNode.getPtr();
			int firstNodeStamp = firstNode.getStamp();
			NonBlockingQueueNode* firstNextNode = firstNodePtr->_next;


			StampPtr lastNode = _tail;
			NonBlockingQueueNode* lastNodePtr = lastNode.getPtr();
			int lastNodeStamp = lastNode.getStamp();
			NonBlockingQueueNode* lastNextNode = lastNodePtr->_next;

			// 이사이에 다른스레드가 dequeue한것
			if (firstNodePtr != _head.getPtr())
			{
				continue;
			}

			// 만약 해드와 테일이 같아지는 순간에
			if (firstNodePtr == lastNodePtr)
			{
				if (nullptr == lastNextNode)
				{
					return false;
				}
				else
				{
					// enqueue과정에서 CAS 1단계만 일어난 상황
					// 여기서 enqueue과정 CAS 2단계 수행해준다.
					STAMPCAS(&_tail, lastNodePtr, lastNodeStamp, lastNextNode);
					// 이제 다시 dequeue할거 찾아야한다.
#ifdef DEBUG_TEST
					{
						NonBlockingQueueNode* test1 = _tail.getPtr();
						NonBlockingQueueNode* test2 = _head.getPtr();
						int a = 3;
					}
#endif
					continue;
				}
			}

			if (nullptr == firstNextNode)
			{
				continue;
			}

			value = firstNextNode->_value;
			// 실패한다면 다른스레드가 dequeue했든, 와서 바꾼것이다.
			if (false == STAMPCAS(&_head, firstNodePtr, firstNodeStamp, firstNextNode))
			{
				continue;
			}

			// first는 이제 큐에서 분리되었기 때문에 안전하게 접근할 수 있다.
			// 또 더이상 사용하지 않으니 delete해준다.
			delete firstNodePtr;
			//cout << " , deq : " << value ;
			return true;

			//////////////////////////////////////////////////////////////////////////
			//StampPtr firstNode = _head;
			//NonBlockingQueueNode* firstNextNode = firstNode._ptr->_next;
			//StampPtr lastNode = _tail;
			//NonBlockingQueueNode* lastNextNode = lastNode._ptr->_next;

			//// 이사이에 다른스레드가 dequeue한것
			//if (firstNode._ptr != _head._ptr)
			//{
			//	continue;
			//}

			//// 만약 해드와 테일이 같아지는 순간에
			//if (firstNode._ptr == lastNode._ptr)
			//{
			//	if (nullptr == lastNextNode)
			//	{
			//		return false;
			//	}

			//	// enqueue과정에서 CAS 1단계만 일어난 상황
			//	// 여기서 enqueue과정 CAS 2단계 수행해준다.
			//	STAMPCAS(&_tail, lastNode._ptr, lastNode._stamp, lastNextNode);
			//	// 이제 다시 dequeue할거 찾아야한다.
			//	continue;
			//}

			//if (nullptr == firstNextNode)
			//{
			//	continue;
			//}

			//value = firstNextNode->_value;
			//// 실패한다면 다른스레드가 dequeue했든, 와서 바꾼것이다.
			//if (false == STAMPCAS(&_head, firstNode._ptr, firstNode._stamp, firstNextNode))
			//{
			//	continue;
			//}

			//// first는 이제 큐에서 분리되었기 때문에 안전하게 접근할 수 있다.
			//// 또 더이상 사용하지 않으니 delete해준다.
			//delete firstNode._ptr;

			//return true;

#else
			NonBlockingQueueNode* firstNode = _head;
			NonBlockingQueueNode* firstNextNode = _head->_next;
			NonBlockingQueueNode* lastNode = _tail;

			// 이사이에 다른스레드가 dequeue한것
			if (firstNode != _head)
			{
				continue;
			}

			// 만약 해드와 테일이 같아지는 순간에 // 비어있거나 enqueue하는 상황이거나
			if (firstNode == lastNode)
			{
				if (nullptr == firstNextNode)
				{
					return false;
				}

				// enqueue과정에서 CAS 1단계만 일어난 상황
				// 여기서 enqueue과정 CAS 2단계 수행해준다.
				CAS(&_tail, lastNode, firstNextNode);
				// 이제 다시 dequeue할거 찾아야한다.
				continue;
			}

			//
			if (nullptr == firstNextNode)
			{
				continue;
			}

			value = firstNextNode->_value;
			// 실패한다면 다른스레드가 dequeue했든, 와서 바꾼것이다.
			if (false == CAS(&_head, firstNode, firstNextNode))
			{
				continue;
			}

			//firstNode->_next = nullptr;


			// first는 이제 큐에서 분리되었기 때문에 안전하게 접근할 수 있다.
			// 또 더이상 사용하지 않으니 delete해준다.
			delete firstNode;

			return true;

			// 예를들어 tail head둘다 보초노드를 가리키고 있다고 했을때
			// enqueue가 CAS 2단계로 나뉘기 때문에 dequeue와 enqueue가 동시에 일어나는 경우
			// (enqueue CAS 1단계 , dequeue CAS완료)와 같은 상황이 생긴다.

			// 원래라면 tail이 아직 보초노드인 상황이기 때문에 큐가 비어있는 것이지만 
			// dequeue할 노드는 enqueue CAS 1단계에서 삽입되었기 때문에 dequeue가 진행된다. 
			// 이렇게 되면 head가 전진하게 되고 tail이 가리키고 있던 노드가 삭제된다. => 크래시
			// 결론 : dequeue할때 enqueue몇단계 상태인지도 알아야한다. 
			//        단순히 next가 nullptr이 아닐때 dequeue하면 안된다.

			//NonBlockingQueueNode* firstNode = _head;

			//// 이사이에 다른스레드가 dequeue한것
			//if (firstNode != _head)
			//{
			//	continue;
			//}

			//// 아무것도없는 상황
			//if (nullptr == firstNode->_next)
			//{
			//	return false;
			//}

			//// 실패한다면 다른스레드가 dequeue했든, 와서 바꾼것이다.
			//if (false == CAS(&_head, firstNode, firstNode->_next))
			//{
			//	continue;
			//}

			//// first는 이제 큐에서 분리되었기 때문에 안전하게 접근할 수 있다.
			//// 또 더이상 사용하지 않으니 delete해준다.
			//value = firstNode->_next->_value;
			//delete firstNode;

			//return true;
#endif
		}
	}

	void NonBlockingQueue::clear(void) noexcept
	{
#ifndef CAS_ONLY

		NonBlockingQueueNode* ptr;
		while (_head.getPtr()->_next != nullptr)
		{
			ptr = _head.getPtr()->_next;
			_head.getPtr()->_next = _head.getPtr()->_next->_next;
			delete ptr;
		}
		_tail = _head;

		///////////////////////////////////////////////////////////
		//NonBlockingQueueNode* ptr;
		//while (_head._ptr->_next != nullptr)
		//{
		//	ptr = _head._ptr->_next;
		//	_head._ptr->_next = _head._ptr->_next->_next;
		//	delete ptr;
		//}
		//_tail = _head;
#else
		NonBlockingQueueNode* ptr;
		while (_head->_next != nullptr)
		{
			ptr = _head->_next;
			_head->_next = _head->_next->_next;
			delete ptr;
		}
		_tail = _head;
#endif
	}

	void NonBlockingQueue::print(void) noexcept
	{
#ifndef CAS_ONLY
		int count = 20;
		NonBlockingQueueNode* ptr = _head.getPtr()->_next;
		while (ptr != nullptr)
		{
			//std::cout << ptr->_value << ", ";
			ptr = ptr->_next;
			--count;
			if (0 == count)
			{
				break;
			}
		}
		//////////////////////////////////////////////////////////////
		//int count = 20;
		//NonBlockingQueueNode* ptr = _head._ptr->_next;
		//while (ptr != nullptr)
		//{
		//	std::cout << ptr->_value << ", ";
		//	ptr = ptr->_next;
		//	--count;
		//	if (0 == count)
		//	{
		//		break;
		//	}
		//}
#else
		int count = 20;
		NonBlockingQueueNode* ptr = _head->_next;
		while (ptr != nullptr)
		{
			std::cout << ptr->_value << ", ";
			ptr = ptr->_next;
			--count;
			if (0 == count)
			{
				break;
			}
		}
		std::cout << endl;
#endif
	}

	bool NonBlockingQueue::CAS(NonBlockingQueueNode* volatile* targetNode
							   , NonBlockingQueueNode* oldNode
							   , NonBlockingQueueNode* newNode) noexcept
	{
		// 해당 주소값을 atomic하게 바꿔준다. // 포인터(64환경에서 8바이트)를 CAS하는 것이기 때문에 unsigned long long사용
		return atomic_compare_exchange_strong(reinterpret_cast<volatile std::atomic<unsigned long long>*>(targetNode)
											  , reinterpret_cast<unsigned long long*>(&oldNode)
											  , reinterpret_cast<unsigned long long> (newNode));
	}

	bool NonBlockingQueue::STAMPCAS(StampPtr* address, NonBlockingQueueNode* oldNode, const int oldStamp, NonBlockingQueueNode* newNode) noexcept
	{
		StampPtr oldStampPtr(oldNode, oldStamp);
		// unsigned short 최대값이 0xFFFF이다 // 상위 16비트를 사용하기때문에
		StampPtr newStampPtr(newNode, (oldStamp + 1 % 0xFFFF));

		// 64 주소값 + 32 스탬프 .. 어떻게할지 고민해보자 64환경에서 64비트 주소값을 32비트로 표현하다가 잘릴수 있다..
		return atomic_compare_exchange_strong(reinterpret_cast<std::atomic<unsigned long long>*>(address)
											  , reinterpret_cast<unsigned long long*>(&oldStampPtr)
											  , *(reinterpret_cast<unsigned long long*>(&newStampPtr)));
	}
}

