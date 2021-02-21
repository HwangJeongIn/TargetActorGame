#include "Common/IdlCompiler.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_set>


namespace ta
{
	// 프로젝트 파일 기준으로 현재디렉토리 결정 // 솔루션 / Input or Output / Idl
	const fs::path IdlInputPath = fs::absolute(L"./../Input/Idl");

	const fs::path IdlOutputServerPath = fs::absolute(L"./../Source/Server");
	const fs::path IdlOutputCommonPath = fs::absolute(L"./../../TAGameClient/Source/TAGameClient/Public/Common");
	const fs::path IdlOutputClientPath = fs::absolute(L"./../../TAGameClient/Source/TAGameClient/Public/Client");
	//const fs::path IdlOutputCommonPath = fs::absolute(L"./../../TAGameClient/Source/TAGameModule/Public/Common");
	//const fs::path IdlOutputClientPath = fs::absolute(L"./../../TAGameClient/Source/TAGameModule/Public/Client");
	//const fs::path IdlOutputCommonPath = fs::absolute(L"./../../TAGameClient/Source/TACommonModule/Common");
	//const fs::path IdlOutputClientPath = fs::absolute(L"./../../TAGameClient/Source/TAClientModule/Client");
	//const fs::path IdlOutputCommonPath = fs::absolute(L"./../../TAGameClient/Source/TACommonModule/Public/Common");
	//const fs::path IdlOutputClientPath = fs::absolute(L"./../../TAGameClient/Source/TAClientModule/Public/Client");
}


namespace ta
{
	IdlCompiler::IdlCompiler(void) noexcept
		: _stcGroup(OriginDestinationType::STC)
		, _ctsGroup(OriginDestinationType::CTS)
	{
	}

	IdlCompiler::~IdlCompiler(void) noexcept
	{
	}

	bool IdlCompiler::gatherIdlFiles(std::vector<fs::path>& idlFiles) noexcept
	{
		if (false == fs::exists(IdlInputPath))
		{
			// 컴파일할 폴더가 없는상황 폴더 만들고 종료하자
			fs::create_directory(IdlInputPath);
			return false;
		}

		for (const fs::directory_entry& entry : fs::directory_iterator(IdlInputPath))
		{
			idlFiles.push_back(entry.path());
		}

		return (0 != idlFiles.size());
	}

	bool IdlCompiler::compileIdlFiles(const std::vector<fs::path>& idlFiles) noexcept
	{
		const size_t count = idlFiles.size();
		for (unsigned int index = 0; index < count; ++index)
		{
			if (false == compileIdlFile(idlFiles[index]))
			{
				return false;
			}
		}

		return true;
	}

	bool IdlCompiler::compileIdlFile(const fs::path& idlFile) noexcept
	{
		std::ifstream in;

		in.open(idlFile);
		if (false == in.is_open())
		{
			return false;
		}

		_currentFileName = idlFile.filename().string();
		TrimExtension(_currentFileName);

		std::string fileString;
		in.seekg(0, std::ios::end); // end에서 0떨어진곳으로 이동
		const long long size = in.tellg();
		in.seekg(0, std::ios::beg); // begin에서 0떨어진곳으로 이동

		fileString.resize(size, 0);
		in.read(&fileString[0], size);
		if (false == convertToPacketGroup(fileString))
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return false;
		}

		createCommonFile();
		createClientFile();
		createServerFile();

#ifdef test
		std::string fileString;

		in.seekg(0, std::ios::end); // end에서 0떨어진곳으로 이동
		//const std::streampos size = in.tellg(); // 시작점부터 위치 반환
		const int size = in.tellg();

		in.seekg(0, std::ios::beg); // begin에서 0떨어진곳으로 이동

		fileString.resize(size, 0);
		in.read(&fileString[0], size);

		// in.getline(buf, 100); // 개행이 나올떄까지 99자 읽음
		// in.getline(buf, 100, '.'); // .이 나올떄까지 99자 읽음
		// getline(in, s); // string용
		in.close();

		// 이제 쓰자
		fs::path outputFile = IdlOutputPath / L"output.h";
		out.open(outputFile);
		if (false == out.is_open())
		{
			return false;
		}

		// 왜인지 모르겠지만 널문자가 여러개 들어간다. 그것까지 파일에 쓰면 이상한문자가 나오는 경우가 있다.
		int a = strlen(fileString.data()); //fileString.length();

		//std::string test("name\nname");
		//out << test;
		out.write(fileString.data(), strlen(fileString.data()));
		//out << fileString;

		out.close();
#endif
		return true;
	}

	bool IdlCompiler::convertToPacketGroup(const std::string& fileString) noexcept
	{
		std::vector<std::string> splitedStrings;
		//std::string input("[CTS]\n{\nSetQuickSlot(const unsigned int& slotNo, const unsigned int& itemSlotNo);\n}\n");
		std::string trimOutput;
		// \t\n을 먼저 없애준다.
		Trim(fileString, trimOutput, "\t\n");
		Split(trimOutput, "[]{}; (),&", splitedStrings);

		// 공백 조각을 없애준다.
		std::vector<std::string>::const_iterator it = splitedStrings.begin();
		while (splitedStrings.end() != it)
		{
			if (0 == it->compare(" "))
			{
				it = splitedStrings.erase(it);
			}
			else
			{
				++it;
			}
		}

		const size_t count = splitedStrings.size();
		for (size_t index = 0; index < count; ++index)
		{
			if (0 == splitedStrings[index].compare("["))
			{
				// [ > CTS or STC
				++index;
				PacketGroup* packetGroup = nullptr;
				if (0 == splitedStrings[index].compare("CTS"))
				{
					packetGroup = &_ctsGroup;
				}
				else if (0 == splitedStrings[index].compare("STC"))
				{
					packetGroup = &_stcGroup;
				}
				else
				{
					TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
					return false;
				}

				/*
				[CTS]
				{
					SetQuickSlot(const unsigned int& slotNo, const unsigned int& itemSlotNo);
					SetEquipSlot(const unsigned int& slotNo, const unsigned int& itemSlotNo);
				}
				*/

				// SetQuickSlot(const unsigned int& slotNo, const unsigned int& itemSlotNo);
				// => functionName > ( > const > type > & > name > , > const > type > & > name > ) > ;

				// CTS or STC > ] > { > functionName
				index += 3;
				std::vector<std::string> splitedPacketStrings;
				while (0 != splitedStrings[index].compare("}"))
				{
					splitedPacketStrings.clear();
					while (0 != splitedStrings[index].compare(";"))
					{
						// SetQuickSlot(const unsigned int& slotNo, const unsigned int& itemSlotNo); 분해되어 들어감
						splitedPacketStrings.push_back(splitedStrings[index++]);
					}

					packetGroup->addPacket(splitedPacketStrings);
					++index;
				}
			}
		}

		return true;
	}

	void IdlCompiler::removeComments(const std::string& fileString, std::string& output) noexcept
	{
	}

	void IdlCompiler::createCommonFile(void) noexcept
	{
		if (false == fs::exists(IdlOutputCommonPath))
		{
			fs::create_directory(IdlOutputCommonPath);
		}

		std::ofstream out;

		const std::vector<std::string>& stcFunctionNames = _stcGroup.getFunctionNames();
		const std::vector<std::vector<std::pair<std::string, std::string>>>& stcParameters = _stcGroup.getParameters();
		const std::vector<std::string>& stcUserDefinedType = _stcGroup.getUserDefinedTypes();

		const std::vector<std::string>& ctsFunctionNames = _ctsGroup.getFunctionNames();
		const std::vector<std::vector<std::pair<std::string, std::string>>>& ctsParameters = _ctsGroup.getParameters();
		const std::vector<std::string>& ctsUserDefinedType = _ctsGroup.getUserDefinedTypes();

		std::string finalString;
		/*인클루드*/
		{
			finalString = "#pragma once\n#include \"Common/MessagePack.h\"\n\n";

						  //\n#include \"Output/Idl/Common/MessageTypeDefinition.h\"\
						  //\n#include \"Common/KeyDefinition.h\"\
						  //\n#include \"Common/ComponentData.h\"\n\n";
		}

		/*전방선언 만들기*/
		{
			std::vector<std::string> userDefinedTypeList;
			{
				std::unordered_set<std::string> userDefinedTypeSet;
				{
					const uint32 count = stcUserDefinedType.size();
					for (uint32 index = 0; index < count; ++index)
					{
						userDefinedTypeSet.insert(stcUserDefinedType[index]);
					}
				}
				{
					const uint32 count = ctsUserDefinedType.size();
					for (uint32 index = 0; index < count; ++index)
					{
						userDefinedTypeSet.insert(ctsUserDefinedType[index]);
					}
				}

				userDefinedTypeList.reserve(userDefinedTypeSet.size());
				for (auto it = userDefinedTypeSet.begin(); it != userDefinedTypeSet.end(); ++it)
				{
					userDefinedTypeList.push_back(*it);
				}
			}

			std::string forwardDeclaration;
			MakeStringWithBracket(userDefinedTypeList, "class", ";", true, forwardDeclaration);

			finalString += "namespace ta\n" + forwardDeclaration + "\n\n\n";
		}

		/* message class 정의*/
		{
			std::string classString;

			{
				const uint32 classCount = stcFunctionNames.size();
				// 무조건 같아야한다.
				if (classCount == stcParameters.size())
				{
					for (uint32 index = 0; index < classCount; ++index)
					{
						makeMessageClassString(stcFunctionNames[index], stcParameters[index], classString);
						finalString += classString + "\n\n\n";
					}
				}
			}

			{
				const uint32 classCount = ctsFunctionNames.size();
				// 무조건 같아야한다.
				if (classCount == ctsParameters.size())
				{
					for (uint32 index = 0; index < classCount; ++index)
					{
						makeMessageClassString(ctsFunctionNames[index], ctsParameters[index], classString);
						finalString += classString + "\n\n\n";
					}
				}
			}
		}

		std::string commonFileName = _currentFileName + "Common.h";
		// 이제 쓰자
		fs::path outputFile = IdlOutputCommonPath / commonFileName;
		out.open(outputFile);
		if (false == out.is_open())
		{
			return;
		}

		// 왜인지 모르겠지만 뒤에 널문자가 여러개 들어간다. 그것까지 파일에 쓰면 이상한문자가 나오는 경우가 있다.
		out.write(finalString.data(), strlen(finalString.data()));
		out.close();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		// 타입 정의 파일 만들기 
		finalString.clear();

		/* 인클루드 */
		{
			finalString = "#pragma once\n#include \"Common/MessagePack.h\"\n\n";
		}
		/* enum type 정의*/
		{
			std::vector<std::string> allFunctionNames;
			allFunctionNames.reserve(stcFunctionNames.size() + ctsFunctionNames.size());
			allFunctionNames.insert(allFunctionNames.end(), stcFunctionNames.begin(), stcFunctionNames.end());
			allFunctionNames.insert(allFunctionNames.end(), ctsFunctionNames.begin(), ctsFunctionNames.end());

			std::string typeSet;
			MakeStringWithBracket(allFunctionNames, "", ",", false, typeSet);

			finalString += "namespace ta\n{\nenum class MessageType : uint32\n" + typeSet + ";\n}\n\n\n";
		}

		std::string messageTypeDefinitionFileName = "MessageTypeDefinition.h";
		// 이제 쓰자
		outputFile = IdlOutputCommonPath / messageTypeDefinitionFileName;
		out.open(outputFile);
		if (false == out.is_open())
		{
			return;
		}

		out.write(finalString.data(), strlen(finalString.data()));
		out.close();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MessageTypeSwtichCaseSet // 스위치 케이스도 자동으로 만들어주자
		finalString.clear();

		/* 인클루드 */
		{
			finalString = "#pragma once\n\n\n";
		}

		/* 매크로 정의 */
		{
			finalString += "#define MESSAGE_SWITCH_CASE(TypeName)\\\n\
							case MessageType::TypeName:\\\n\
							{\\\n\
								TypeName::processMessage(static_cast<const TypeName*>(message));\\\n\
							}\\\n\
							break;\\\n\n";
		}

		/* 각 Case 정의*/
		{
			
			std::vector<std::string> allFunctionNames;
			allFunctionNames.reserve(stcFunctionNames.size() + ctsFunctionNames.size());
			allFunctionNames.insert(allFunctionNames.end(), stcFunctionNames.begin(), stcFunctionNames.end());
			allFunctionNames.insert(allFunctionNames.end(), ctsFunctionNames.begin(), ctsFunctionNames.end());

			const uint32 count = allFunctionNames.size();
			for (uint32 index = 0; index < count; ++index)
			{
				finalString += "MESSAGE_SWITCH_CASE(" + allFunctionNames[index] + ")\n";
			}

			finalString += "\n\n#undef MESSAGE_SWITCH_CASE\n";
		}

		std::string messageTypeSwtichCaseSetFileName = "MessageTypeSwtichCaseSet.h";
		// 이제 쓰자
		outputFile = IdlOutputCommonPath / messageTypeSwtichCaseSetFileName;
		out.open(outputFile);
		if (false == out.is_open())
		{
			return;
		}

		out.write(finalString.data(), strlen(finalString.data()));
		out.close();
	}

	void IdlCompiler::createServerFile(void) noexcept
	{
		createSendFile(OriginDestinationType::STC);
	}

	void IdlCompiler::createClientFile(void) noexcept
	{
		createSendFile(OriginDestinationType::CTS);
	}
	
	void IdlCompiler::createSendFile(const OriginDestinationType& type) noexcept
	{
		fs::path targetPath;
		std::string name;
		PacketGroup* packetGroup = nullptr;

		switch (type)
		{
		case OriginDestinationType::STC:
			{
				targetPath = IdlOutputServerPath;
				name = "Server";
				packetGroup = &_stcGroup;
			}
			break;
		case OriginDestinationType::CTS:
			{
				targetPath = IdlOutputClientPath;
				name = "Client";
				packetGroup = &_ctsGroup;
			}
			break;
		}

		if (false == fs::exists(targetPath))
		{
			fs::create_directory(targetPath);
		}

		const std::vector<std::string>& functionNames = packetGroup->getFunctionNames();
		const std::vector<std::vector<std::pair<std::string, std::string>>>& parameters = packetGroup->getParameters();
		const std::vector<std::string>& userDefinedType = packetGroup->getUserDefinedTypes();
		std::string finalString;

		/*인클루드*/
		{
			finalString = "#pragma once\n#include \"Common/" + _currentFileName + "Common.h\"\
						  \n#include \"" + name + "/" + name + "App.h\"\
						  \n#include \"" + name + "/" + name + "ActorManager.h\"\
						  \n#include \"Common/CommonActor.h\"\
						  \n#include \"Common/Socket.h\" \n\n";
		}

		/*전방선언 만들기*/
		{
			std::vector<std::string> userDefinedTypeList;
			{
				std::unordered_set<std::string> userDefinedTypeSet;
				{
					const uint32 count = userDefinedType.size();
					for (uint32 index = 0; index < count; ++index)
					{
						userDefinedTypeSet.insert(userDefinedType[index]);
					}
				}

				userDefinedTypeList.reserve(userDefinedTypeSet.size());
				for (auto it = userDefinedTypeSet.begin(); it != userDefinedTypeSet.end(); ++it)
				{
					userDefinedTypeList.push_back(*it);
				}
				userDefinedTypeList.push_back("ActorKey");
			}

			std::string forwardDeclaration;
			MakeStringWithBracket(userDefinedTypeList, "class", ";", true, forwardDeclaration);

			finalString += "namespace ta\n" + forwardDeclaration + "\n\n\n";
		}

		/* send 정의*/
		{
			std::string sendString;
			const uint32 classCount = functionNames.size();
			// 무조건 같아야한다.
			if (classCount == parameters.size())
			{
				for (uint32 index = 0; index < classCount; ++index)
				{
					makeSendString(functionNames[index], parameters[index], sendString);
					finalString += sendString + "\n\n\n";
				}
			}
		}

		/*파일 생성*/
		std::ofstream out;

		std::string serverFileName = _currentFileName + name + ".h";
		fs::path outputFile = targetPath / serverFileName;
		out.open(outputFile);
		if (false == out.is_open())
		{
			return;
		}

		// 왜인지 모르겠지만 뒤에 널문자가 여러개 들어간다. 그것까지 파일에 쓰면 이상한문자가 나오는 경우가 있다.
		out.write(finalString.data(), strlen(finalString.data()));
		out.close();
	}

	void IdlCompiler::makeMessageClassString(const std::string& functionName
											, const std::vector<std::pair<std::string, std::string>>& parameters
											, std::string& output) const noexcept
	{
		output.clear();

		std::string inputParameterBracketStringWithoutNetworkActorKey;
		{
			makeInputParameterBracketString(parameters, inputParameterBracketStringWithoutNetworkActorKey);
		}

		std::string inputParameterBracketString;
		{
			std::vector<std::pair<std::string, std::string>> inputParameters;
			const uint32 count = parameters.size();
			inputParameters.reserve(count + 1);
			inputParameters.push_back(std::make_pair<std::string, std::string>("ActorKey", "networkActorKey"));
			for (uint32 index = 0; index < count; ++index)
			{
				inputParameters.push_back(parameters[index]);
			}
			makeInputParameterBracketString(inputParameters, inputParameterBracketString);
		}

		std::string inputArgumentBracketString;
		{
			std::vector<std::pair<std::string, std::string>> inputArguments;
			const uint32 count = parameters.size();
			inputArguments.reserve(count + 1);
			inputArguments.push_back(std::make_pair<std::string, std::string>("ActorKey", "message->_networkActorKey"));
			for (uint32 index = 0; index < count; ++index)
			{
				inputArguments.push_back(std::make_pair<std::string, std::string>(parameters[index].first.data(), "message->" + parameters[index].second));
			}
			makeInputArgumentBracketString(inputArguments, inputArgumentBracketString);
		}

		std::string initializerString;
		makeInitializerString(functionName, parameters, initializerString);

		std::string memberString;
		makeMemberString(parameters, memberString);

		output = // "namespace ta\n{\n\t#pragma pack(push, 1)\n\tclass " + functionName
			"namespace ta\n{\n\tclass " + functionName
			+ " : public MessagePack\n\t{\n\tpublic:\n\t\t explicit "
			+ functionName + '\n' + inputParameterBracketStringWithoutNetworkActorKey
			+ " noexcept\n" + initializerString + " {}\n\t\tvirtual ~" + functionName
			+ "(void) noexcept {}\n\t\tstatic void processMessage(const " + functionName + "* message) noexcept \n{ \n"
			+ functionName + "::process"
			+ functionName + '\n' + inputArgumentBracketString + ";\n}\n\t\tstatic void process"
			+ functionName + '\n' + inputParameterBracketString + " noexcept;\n\n\tpublic:\n"
			+ memberString + "\n\t};\n}";
	}
	
	void IdlCompiler::makeInitializerString(const std::string functionName
											, const std::vector<std::pair<std::string, std::string>>& parameters
											, std::string& output) const noexcept
	{
		/*
			: _messageType(MessageType::SetQuickSlotCTS),
			slotNo(slotNo),
			itemSlotNo(itemSlotNo)
		*/
		output.clear();
		output = "\t\t\t: MessagePack(MessageType::" + functionName + "),\n";
		
		std::vector<std::string> nameSet;
		const uint32 count = parameters.size();
		nameSet.reserve(count);
		for (uint32 index = 0; index < count; ++index)
		{
			nameSet.push_back(parameters[index].second + "(" + parameters[index].second + ")");
		}

		std::string parameterSet;
		MakeStringWithBracket(nameSet, "\t\t", ",", false, parameterSet, BracketType::None);

		output += parameterSet;
	}

	void IdlCompiler::makeInputParameterBracketString(const std::vector<std::pair<std::string, std::string>>& parameters, std::string& output) const noexcept
	{
		/*
		(const unsigned int& slotNo, 
		const unsigned int& itemSlotNo)
		*/

		output.clear();

		std::vector<std::string> typeNameSet;
		const uint32 count = parameters.size();
		typeNameSet.reserve(count);
		for (uint32 index = 0; index < count; ++index)
		{
			typeNameSet.push_back("const " + parameters[index].first + "& " + parameters[index].second);
		}

		MakeStringWithBracket(typeNameSet, "", ",", false, output, BracketType::Round);
	}

	void IdlCompiler::makeInputArgumentBracketString(const std::vector<std::pair<std::string, std::string>>& parameters, std::string& output) const noexcept
	{
		/*
		(slotNo,
		itemSlotNo)
		*/

		output.clear();

		std::vector<std::string> typeNameSet;
		const uint32 count = parameters.size();
		typeNameSet.reserve(count);
		for (uint32 index = 0; index < count; ++index)
		{
			typeNameSet.push_back(parameters[index].second);
		}

		MakeStringWithBracket(typeNameSet, "", ",", false, output, BracketType::Round);
	}

	void IdlCompiler::makeMemberString(const std::vector<std::pair<std::string, std::string>>& parameters, std::string& output) const noexcept
	{
		/*
		unsigned int slotNo;
		unsigned int itemSlotNo;
		*/

		output.clear();

		std::vector<std::string> typeNameSet;
		const uint32 count = parameters.size();
		typeNameSet.reserve(count);
		for (uint32 index = 0; index < count; ++index)
		{
			typeNameSet.push_back(parameters[index].first + " " + parameters[index].second);
		}

		MakeStringWithBracket(typeNameSet, "\t", ";", true, output, BracketType::None);
	}
	
	void IdlCompiler::makeSendString(const std::string functionName
									 , const std::vector<std::pair<std::string, std::string>>& parameters
									 , std::string& output) const noexcept
	{
		output.clear();

		std::string inputParameterBracketString;
		{
			std::vector<std::pair<std::string, std::string>> inputParameters;
			const uint32 count = parameters.size();
			inputParameters.reserve(count + 1);
			inputParameters.push_back(std::make_pair<std::string, std::string>("ActorKey", "networkActorKey"));
			for (uint32 index = 0; index < count; ++index)
			{
				inputParameters.push_back(parameters[index]);
			}

			makeInputParameterBracketString(inputParameters, inputParameterBracketString);
		}

		std::string inputArgumentBracketString;
		{	
			makeInputArgumentBracketString(parameters, inputArgumentBracketString);

			//std::vector<std::pair<std::string, std::string>> inputArguments;
			//const uint32 count = parameters.size();
			//inputArguments.reserve(count + 1);
			//inputArguments.push_back(std::make_pair<std::string, std::string>("ActorKey", "networkActorKey"));
			//for (uint32 index = 0; index < count; ++index)
			//{
			//	inputArguments.push_back(std::make_pair<std::string, std::string>(parameters[index].first.data(), parameters[index].second.data()));
			//}
			//makeInputArgumentBracketString(inputArguments, inputArgumentBracketString);
		}

		std::string initializerString;
		makeInitializerString(functionName, parameters, initializerString);

		std::string memberString;
		makeMemberString(parameters, memberString);

		output =
			"namespace ta\n{\n\tstatic void Send"
			+ functionName + '\n' + inputParameterBracketString
			+ "\n\t{\n\t\t" + functionName + " message\n" + inputArgumentBracketString
			+ ";\n\t\tmessage._length = sizeof(" + functionName + ");"
			+ "\n\t\tmessage._networkActorKey = networkActorKey;"
			+ "\n\t\tCommonActor* target = GetActorManager()->getActor(networkActorKey);\n\t\tif(nullptr == target)\
			\n\t\t{\n\t\t\treturn;\n\t\t}\n\t\tSocket* network = target->getNetwork_();\n\t\tif(nullptr == network)\
			\n\t\t{\n\t\t\treturn;\n\t\t}\n\t\tnetwork->sendOverlapped(&message);\n\t}\n}";
	}
}


namespace ta
{
	PacketGroup::PacketGroup(const OriginDestinationType& type) noexcept
		: _type(type)
	{
	}

	PacketGroup::~PacketGroup(void) noexcept
	{
	}

	const OriginDestinationType PacketGroup::getType(void) const noexcept
	{
		return _type;
	}

	void PacketGroup::clear(void) noexcept
	{
		_userDefinedTypes.clear();
		_parameters.clear();
		_functionNames.clear();
	}

	void PacketGroup::addPacket(const std::vector<std::string>& splitedStrings) noexcept
	{
		const size_t count = splitedStrings.size();
		// 파라미터가 하나도 없어도 최소 4개는 나온다. void 명시했을때
		if (4 > count)
		{
			TA_ASSERT_DEV(false, "비정상적인 상황입니다.");
			return;
		}

		_functionNames.push_back(_type == OriginDestinationType::CTS? (splitedStrings[0] + "CTS") : (splitedStrings[0] + "STC"));
		_parameters.push_back(std::vector<std::pair<std::string, std::string>>());

		// void parameter일때
		if (4 == count)
		{
			return;
		}

		// functionName > ( > const > type
		size_t index = 3;
		while (index < count)
		{
			// => functionName > ( > const > type > & > name > , > const > type > & > name > )
			std::string typeString;
			while (0 != splitedStrings[index].compare("&"))
			{
				typeString += splitedStrings[index] + " ";
				++index;
			}

			typeString.erase(typeString.size() - 1);
			if (true == isUserDefinedType(typeString))
			{
				_userDefinedTypes.push_back(typeString);
			}

			// & > name
			++index;
			_parameters.back().push_back(std::make_pair<std::string, std::string>(typeString.data(), splitedStrings[index].data()));

			// name > , or )
			++index;
			if (0 == splitedStrings[index].compare(","))
			{
				// , > const > type
				index += 2;
				continue;
			}

			// )
			break;
		}
	}

	void PacketGroup::addPacket(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& parameters) noexcept
	{
		_functionNames.push_back(functionName);
		_parameters.push_back(parameters);
	}

	const std::vector<std::vector<std::pair<std::string, std::string>>>& PacketGroup::getParameters(void) noexcept
	{
		return _parameters;
	}

	const std::vector<std::string>& PacketGroup::getFunctionNames(void) noexcept
	{
		return _functionNames;
	}

	const std::vector<std::string>& PacketGroup::getUserDefinedTypes(void) noexcept
	{
		return _userDefinedTypes;
	}

	const bool PacketGroup::isUserDefinedType(const std::string& type) noexcept
	{
		// 비교하는것중에 하나라도 맞으면 유저정의타입이 아니다. // 다른방법을 찾아보자
		return false == (
			(0 == type.compare("uint64"))
			||(0 == type.compare("uint32"))
			||(0 == type.compare("uint8"))
			||(0 == type.compare("int64"))
			||(0 == type.compare("int32"))
			||(0 == type.compare("int8"))
			||(0 == type.compare("char"))
			||(0 == type.compare("unsigned int"))
			||(0 == type.compare("int"))
			);
	}
}
