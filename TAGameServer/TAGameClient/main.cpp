#include "Client/ClientApp.h"


int main(void)
{
	_wsetlocale(LC_ALL, L"Korean");

	ta::g_app->run();

	delete ta::g_app;
	return 0;
}