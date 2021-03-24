#include <iostream>
#include "Server/ServerApp.h"
//#include "Common/CommonActionActorComponent.h"
#include <vector>
#include "Common/CommonBase.h"

#include <string>

//#pragma comment(lib, "CommonLibrary.lib")

using namespace std;

#define TESTA

int main(void)
{
	_wsetlocale(LC_ALL, L"Korean");

	char test[] = "ab";
	wchar_t wtest[] = L"ab";
	std::cout << test <<" , "<< strlen(test) <<std::endl;
	std::cout << wtest << " , " << lstrlenW(wtest) <<std::endl;


	//TA_LOG_DEV("33 hello 테스트 %d", 3);

	ta::g_app->run();

	//////////////////////////////////////////////////////////////////////

	//vector<int> test;
	////test.back();

	//std::hash<int> aaa;

	//std::hash<ta::ActorKey> a;
	//std::equal_to<ta::ActorKey> b;

	//////////////////////////////////////////////////////////////////////

	//std::shared_ptr<int> _poolValues[8];
	//_poolValues[0] = make_shared<int>(5);
	//_poolValues[1] = make_shared<int>(50);

	//cout << _poolValues[0].get() << endl;
	//cout << _poolValues[1].get() << endl;


	//////////////////////////////////////////////////////////////////////
	//std::vector<int> test;
	//test.reserve(2);
	//test.push_back(3);
	//test.push_back(2);

	//cout << &test[0] << endl;
	//
	//test.push_back(5);
	////test.reserve(5);
	//
	//
	//cout << &test[0] << endl;
	////////////////////////////////////////////////////////////////////
	//TA_ASSERT_DEV(false, "33 hello 테스트 %d", 3);

	//std::vector<ta::ActorComponent*> test;
	//test.push_back(new ta::CommonActionActorComponent);
	//std::vector<ta::CommonActionActorComponent*> test2;
	//test2 = (std::vector<ta::CommonActionActorComponent*>)(test);

	//const uint32 count = test.size();
	//for (uint32 index = 0; index < count; ++index)
	//{
	//	delete test[index];
	//}
//	auto a = TA_LOG_CALLINFO;
//	cout << TA_LOG_CALLINFO;

	delete ta::g_app;
	return 0;
}