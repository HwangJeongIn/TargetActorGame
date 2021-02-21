#include <iostream>
#include "Common/IdlCompiler.h"

int main(void)
{
	ta::IdlCompiler compiler;
	std::vector<fs::path> _idlFiles;
	if (true == compiler.gatherIdlFiles(_idlFiles))
	{
		compiler.compileIdlFiles(_idlFiles);
	}

	//fs::path p = ta::IdlInputPath;
	//fs::exists(p);
	//auto p2 = p.relative_path();
	//auto p3 = fs::absolute(p);
	//auto p4 = fs::canonical(p);

	//std::cout << "내 현재 경로 : " << fs::current_path() << std::endl;
	//std::cout << "상대 경로 : " << p.relative_path() << std::endl;
	//std::cout << "절대 경로 : " << fs::absolute(p) << std::endl;
	//std::cout << "공식적인 절대 경로 : " << fs::canonical(p) << std::endl;

	//fs::path finalIdlPath = ProjectPath;
	//finalIdlPath += IdlPath;

	//for (const auto& entry : fs::directory_iterator(finalIdlPath))
	//	std::cout << entry.path() << std::endl;



	return 0;
}