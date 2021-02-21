#pragma once

#include "Common/CommonBase.h"
#include <filesystem>
#include <string>
#include <vector>


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
	extern void Split(const std::string& input, const std::string& delimiters, std::vector<std::string>& output) noexcept;

	extern void MakeStringWithBracket(const std::vector<std::string>& items
									  , const std::string& itemHeader
									  , const std::string& separator
									  , const bool hasLastSeparator
									  , std::string& output
									  , const BracketType bracketType = BracketType::Curly) noexcept;

	extern tstring ToTstring(const std::string& input) noexcept;
	extern tstring ToTstring(const std::wstring& input) noexcept;

	extern bool StringWithBracketToVector(const std::string& input, Vector& output) noexcept;
	extern bool StringToVector(const std::string& input, Vector& output) noexcept;


	extern int8 Atoi8(const std::string& input) noexcept;
	extern int32 Atoi(const std::string& input) noexcept;
	extern int64 Atoi64(const std::string& input) noexcept;

	extern uint8 Atoui8(const std::string& input) noexcept;
	extern uint32 Atoui(const std::string& input) noexcept;
	extern uint64 Atoui64(const std::string& input) noexcept;

	extern float Atof(const std::string& input) noexcept;
	extern bool Atob(const std::string& input) noexcept; // "1" / "0" 구분
	extern bool Atobs(const std::string& input) noexcept; // "true" / "false" 구분 // string ver

}
