#pragma once

#include "Common/CommonBase.h"
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>


namespace fs = std::filesystem;

namespace ta
{
	class Vector;
}


namespace ta
{
	extern hash_value ComputeHash(const std::string& input) noexcept;
	extern hash_value ComputeHash(const std::wstring& input) noexcept;

	extern void Trim(const std::string& input, std::string& output, const std::string& targetCharacters) noexcept;
	extern void TrimEmpty(const std::string& input, std::string& output) noexcept;
	extern void TrimExtension(std::string& inOut) noexcept;
	extern void Extension(const std::string& input, std::string& output) noexcept;
	extern void Split(const std::string& input, const std::string& delimiters, std::vector<std::string>& output) noexcept;

	extern void MakeStringWithBracket(const std::vector<std::string>& items
									  , const std::string& itemHeader
									  , const std::string& separator
									  , const bool hasLastSeparator
									  , std::string& output
									  , const BracketType bracketType = BracketType::Curly) noexcept;

	extern std::wstring StringToWstring(const std::string& input) noexcept;
	extern std::string WstringToString(const std::wstring& input) noexcept;


	extern tstring ToTstring(const std::string& input) noexcept;
	extern tstring ToTstring(const std::wstring& input) noexcept;

	extern std::string ToString(const tstring& input) noexcept;
	extern std::wstring ToWstring(const tstring& input) noexcept;

	extern bool StringWithBracketToVector(const std::string& input, Vector& output) noexcept;
	extern bool StringToVector(const std::string& input, Vector& output) noexcept;

	template<typename T> std::string ToStringCast(const T& input) noexcept
	{
		std::ostringstream os;
		os << input;
		return os.str();
	}

	template<typename T> T FromStringCast(const std::string& input) noexcept
	{
		T rv = 0;
		std::istringstream(input.c_str()) >> rv;
		return rv;
	}

	template<> bool FromStringCast(const std::string& input) noexcept;

	//extern int8 Atoi8(const std::string& input) noexcept;
	//extern int32 Atoi(const std::string& input) noexcept;
	//extern int64 Atoi64(const std::string& input) noexcept;

	//extern uint8 Atoui8(const std::string& input) noexcept;
	//extern uint32 Atoui(const std::string& input) noexcept;
	//extern uint64 Atoui64(const std::string& input) noexcept;

	//extern float Atof(const std::string& input) noexcept;
	//extern bool Atob(const std::string& input) noexcept; // "1" / "0" 구분
	//extern bool Atobs(const std::string& input) noexcept; // "true" / "false" 구분 // string ver

	//extern std::string FormatString(const char* format, ...) noexcept;
}
