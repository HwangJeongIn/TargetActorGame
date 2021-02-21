#pragma once

#include "Common/CommonBase.h"
#include "Common/StringUtility.h"




namespace ta
{
	extern const fs::path IdlInputPath;
	extern const fs::path IdlOutputServerPath;
	extern const fs::path IdlOutputCommonPath;
	extern const fs::path IdlOutputClientPath;
}


namespace ta
{
	class PacketGroup
	{
	public:
		explicit PacketGroup(const OriginDestinationType& type) noexcept;
		~PacketGroup(void) noexcept;

		const OriginDestinationType getType(void) const noexcept;
		void clear(void) noexcept;

		void addPacket(const std::vector<std::string>& splitedStrings) noexcept;
		void addPacket(const std::string& functionName, const std::vector<std::pair<std::string, std::string>>& parameters) noexcept;

		const std::vector<std::vector<std::pair<std::string, std::string>>>& getParameters(void) noexcept;
		const std::vector<std::string>& getFunctionNames(void) noexcept;
		const std::vector<std::string>& getUserDefinedTypes(void) noexcept;



	private:
		const bool isUserDefinedType(const std::string& type) noexcept;


	private:
		OriginDestinationType _type;

		// 인덱스가 동일하면 같은 패킷 정의 함수
		std::vector<std::vector<std::pair<std::string, std::string>>/*Type, Name*/> _parameters;
		std::vector<std::string> _functionNames;

		// 위에서 유저정의 타입을 모아놓은것 // 전방선언에 사용됨
		std::vector<std::string> _userDefinedTypes;
	};
}


namespace ta
{
	class IdlCompiler
	{
	public:
		IdlCompiler(void) noexcept;
		~IdlCompiler(void) noexcept;

		bool gatherIdlFiles(std::vector<fs::path>& idlFiles) noexcept;
		bool compileIdlFiles(const std::vector<fs::path>& idlFiles) noexcept;
		bool compileIdlFile(const fs::path& idlFile) noexcept;
	
	private:
		bool convertToPacketGroup(const std::string& fileString) noexcept;
		void removeComments(const std::string& fileString, std::string& output) noexcept;
		
		void createCommonFile(void) noexcept;
		void createServerFile(void) noexcept;
		void createClientFile(void) noexcept;

		void createSendFile(const OriginDestinationType& type) noexcept;

		void makeMessageClassString(const std::string& functionName
									, const std::vector<std::pair<std::string, std::string>>& parameters
									, std::string& output) const noexcept;


		void makeInitializerString(const std::string functionName
								   , const std::vector<std::pair<std::string, std::string>>& parameters
								   , std::string& output) const noexcept;

		void makeInputParameterBracketString(const std::vector<std::pair<std::string, std::string>>& parameters
											 , std::string& output) const noexcept;

		void makeInputArgumentBracketString(const std::vector<std::pair<std::string, std::string>>& parameters
											, std::string& output) const noexcept;

		void makeMemberString(const std::vector<std::pair<std::string, std::string>>& parameters
							  , std::string& output) const noexcept;

		void makeSendString(const std::string functionName
							, const std::vector<std::pair<std::string, std::string>>& parameters
							, std::string& output) const noexcept;

	private:
		std::string _currentFileName;
		PacketGroup _stcGroup;
		PacketGroup _ctsGroup;
	};
}
