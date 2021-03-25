#include "Common/StringUtility.h"
#include "Common/Vector.h"
#include <cstdlib>
#include <unordered_map>
#include <locale>
#include <codecvt>


namespace ta
{
	std::hash<std::string> StringHasher;
	std::hash<std::wstring> WstringHasher;

	hash_value ComputeHash(const std::string& input) noexcept
	{
		return StringHasher(input);
	}

	hash_value ComputeHash(const std::wstring& input) noexcept
	{
		return WstringHasher(input);
	}

	void Trim(const std::string& input, std::string& output, const std::string& targetCharacters) noexcept
	{
		output.clear();
		const size_t size = input.size();
		const size_t targetCharacterCount = targetCharacters.size();

		output.reserve(size);

		for (size_t index = 0; index < size; ++index)
		{
			size_t targetIndex = 0;
			for (; targetIndex < targetCharacterCount; ++targetIndex)
			{
				if (input[index] == targetCharacters[targetIndex])
				{
					break;
				}
			}

			// 하나라도 겹치는게 있으면 삽입하지 않는다.
			if (targetIndex < targetCharacterCount)
			{
				continue;
			}

			output += input[index];
		}

		output.shrink_to_fit();
	}

	void TrimEmpty(const std::string& input, std::string& output) noexcept
	{
		Trim(input, output, "\t\n ");
	}

	void TrimExtension(std::string& inOut) noexcept
	{
		uint32 index = inOut.size();
		if (0 == index)
		{
			return;
		}

		--index;

		while (0 <= index)
		{
			if ('.' == inOut[index])
			{
				inOut.resize(index);
				return;
			}
			--index;
		}
	}

	void Extension(const std::string& input, std::string& output) noexcept
	{
		const uint32 size = input.size();
		if (0 == size)
		{
			return;
		}

		output.clear();
		int32 index = static_cast<int32>(size);
		--index;

		while (0 <= index)
		{
			if ('.' == input[index])
			{
				output = input.substr(index + 1, size - (index+1));
				return;
			}
			--index;
		}
	}

	void Split(const std::string& input, const std::string& delimiters, std::vector<std::string>& output) noexcept
	{
		//std::vector<std::string> strings;
		//std::istringstream f("denmark;sweden;india;us");
		//std::string s;
		//while (std::getline(f, s, ';'))
		//{
		//	std::cout << s << std::endl;
		//	strings.push_back(s);
		//}

		output.clear();

		std::string targetString = input;

		std::string::size_type pos = 0;
		std::string::size_type prevPos = 0;
		const std::string::size_type length = targetString.length();

		char delimiter[2]{ NULL };
		while (prevPos < length)
		{
			// 찾은 것 중에 가장 앞에있는 위치 얻어온다.
			pos = targetString.find_first_of(delimiters, prevPos);
			if (pos == std::string::npos)
			{
				// 없으면 마지막까지 설정해서 넣어준다.
				output.push_back(targetString.substr(prevPos, length - prevPos));
				return;
			}

			if (pos != prevPos)
			{
				output.push_back(targetString.substr(prevPos, pos - prevPos));
			}

			// 구분자도 같이 푸시해야한다.
			delimiter[0] = targetString[pos];
			output.push_back(std::string(delimiter));

			prevPos = pos + 1;
		}
		int a = 3;
	}

	void MakeStringWithBracket(const std::vector<std::string>& items
							   , const std::string& itemHeader
							   , const std::string& separator
							   , const bool hasLastSeparator
							   , std::string& output
							   , const BracketType bracketType/*= BracketType::Curly*/) noexcept
	{
		char bracket[3]{ "  " };
		switch (bracketType)
		{
		case BracketType::Curly:
			{
				bracket[0] = '{';
				bracket[1] = '}';
			}
			break;
		case BracketType::Round:
			{
				bracket[0] = '(';
				bracket[1] = ')';
			}
			break;
		case BracketType::Square:
			{
				bracket[0] = '[';
				bracket[1] = ']';
			}
			break;
		}

		output.clear();
		const uint32 count = items.size();
		if (0 == count)
		{
			output += bracket;
			return;
		}

		output = bracket[0];

		for (uint32 index = 0; index < count; ++index)
		{
			output += itemHeader + ' ' + items[index] + separator + '\n';
		}


		if (false == hasLastSeparator)
		{
			// remove separater + \n
			output.resize(output.size() - (separator.size() + 1));
			//output.erase(output.size() - (separator.size() + 1), (separator.size() + 1));
		}
		else
		{
			// remove \n
			output.resize(output.size() - 1);
		}

		output += bracket[1];
	}

	

	std::wstring StringToWstring(const std::string& input) noexcept
	{
		wchar_t outputTemp[256]{ 0, };
		if (false == CharToWchar(input.c_str(), outputTemp, 256))
		{
			return std::wstring();
		}

		return std::wstring(outputTemp);
	}

	std::string WstringToString(const std::wstring& input) noexcept
	{
		char outputTemp[256]{ 0, };
		if (false == WcharToChar(input.c_str(), outputTemp, 256))
		{
			return std::string();
		}

		return std::string(outputTemp);
	}

	tstring ToTstring(const std::string& input) noexcept
	{
#ifdef UNICODE // string => wstring
		return StringToWstring(input);
#else // string => string
		return input;
#endif
	}

	tstring ToTstring(const std::wstring& input) noexcept
	{
#ifdef UNICODE // wstring => wstring
		return input;
#else // wstring => string
		return WstringToString(input);
#endif
	}

	std::string ToString(const tstring& input) noexcept
	{
#ifdef UNICODE // wstring => string 
		return WstringToString(input);
#else // string => string
		return input;
#endif
	}

	std::wstring ToWstring(const tstring& input) noexcept
	{
#ifdef UNICODE // wstring => wstring 
		return input;
#else // string => wstring
		return StringToWstring(input);
#endif
	}

	bool StringWithBracketToVector(const std::string& input, Vector& output) noexcept
	{
		std::vector<std::string> splitedStrings;
		Split(input, "(,)", splitedStrings); // (  x  ,  y  ,  z  )
		const uint32 splitedStringCount = splitedStrings.size();
		if (7 != splitedStringCount)
		{
			TA_ASSERT_DEV(false, "비정상적인 String입니다.");
			return false;
		}

		output._x = FromStringCast<float>(splitedStrings[1]);
		output._y = FromStringCast<float>(splitedStrings[3]);
		output._z = FromStringCast<float>(splitedStrings[5]);

		return true;
	}

	bool StringToVector(const std::string& input, Vector& output) noexcept
	{
		std::vector<std::string> splitedStrings;
		Split(input, ",", splitedStrings); //   x  ,  y  ,  z  
		const uint32 splitedStringCount = splitedStrings.size();
		if (5 != splitedStringCount)
		{
			TA_ASSERT_DEV(false, "비정상적인 String입니다.");
			return false;
		}

		output._x = FromStringCast<float>(splitedStrings[0]);
		output._y = FromStringCast<float>(splitedStrings[2]);
		output._z = FromStringCast<float>(splitedStrings[4]);

		return true;
	}

	template<> bool FromStringCast(const std::string& input) noexcept
	{
		bool rv = false;
		if (1 < input.size())
		{
			std::istringstream(input.c_str()) >> std::boolalpha >> rv;
		}
		else
		{
			std::istringstream(input.c_str()) >> rv;
		}
		
		return rv;
	}
	
	//int8 Atoi8(const std::string& input) noexcept
	//{
	//	int8 rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//int32 Atoi(const std::string& input) noexcept
	//{
	//	int32 rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//int64 Atoi64(const std::string& input) noexcept
	//{
	//	int64 rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//uint8 Atoui8(const std::string& input) noexcept
	//{
	//	uint8 rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//uint32 Atoui(const std::string& input) noexcept
	//{
	//	uint32 rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//uint64 Atoui64(const std::string& input) noexcept
	//{
	//	uint64 rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}
	//
	//float Atof(const std::string& input) noexcept
	//{
	//	float rv = 0;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//bool Atob(const std::string& input) noexcept
	//{
	//	bool rv = false;
	//	std::istringstream(input.c_str()) >> rv;
	//	return rv;
	//}

	//bool Atobs(const std::string& input) noexcept
	//{
	//	bool rv = false;
	//	std::istringstream(input.c_str()) >> std::boolalpha >> rv;
	//	return rv;
	//}

	//std::string FormatString(const char* format, ...) noexcept
	//{
	//	va_list list;
	//
	//	char data[256];
	//
	//	va_start(list, format);
	//	sprintf_s(data, 256, format, list);
	//	va_end(list);
	//}
}