#pragma once


#ifndef TA_SERVER
#include "Windows/AllowWindowsPlatformTypes.h"
//#include "Windows/HideWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "TAGameClient.h"
//#pragma warning(disable:5038) // 초기화순서 안맞을 수 있음
//#pragma error(disable:4495) // __super사용 못하게 되어있음
//#pragma error(disable:4211) // 키 클래스 매크로 
#endif

#define _WINSOCKAPI_ // winsock2사용
#include <Windows.h>


//
//#ifdef TA_CONSOLE_SERVER

	#define MAX_LOG_SIZE 256
	#include <string>
	#include <cstdio>
	#include <iostream>
	
	#ifdef UNICODE
		
		#define Sprint swprintf_s
		//#define TString std::wstring
		//#define ToString std::to_wstring
		#define PrintLog wprintf_s
	#else
		#define Sprint sprintf_s
		//#define TString std::string
		//#define ToString std::to_string
		#define PrintLog printf_s
	#endif

//#define	ToTString(raw) TString(raw)



//#endif

namespace ta
{
	typedef std::basic_string<TCHAR> tstring;

	static bool CharToTChar(const char* origin, TCHAR* output, const int outputMaxLen)
	{
#ifdef UNICODE
		if (nullptr != origin && nullptr != output)
		{
			const int originLen = (int)strlen(origin);
			// lpWideCharStr.If this value is 0, the function returns the required buffer size,
			const int outputLen = MultiByteToWideChar(CP_ACP, 0, origin, -1, NULL, 0);
			if (outputMaxLen < outputLen)
			{
				DebugBreak();
				return false;
			}

			const bool isSucceeded = (0 != MultiByteToWideChar(CP_ACP, 0, origin, originLen + 1, output, outputLen));

			return isSucceeded;
		}

		return false;
#else
		return (0 == strcpy_s(output, outputMaxLen, origin));
#endif
	}

	static bool TCharToChar(const TCHAR* origin, char* output, const int outputMaxLen)
	{
#ifdef UNICODE
		if (nullptr != origin && nullptr != output)
		{
			// If the function succeeds and cbMultiByte is 0, the return value is the required size
			const int outputLen = WideCharToMultiByte(CP_ACP, 0, origin, -1, NULL, 0, NULL, NULL);
			if (outputMaxLen < outputLen)
			{
				DebugBreak();
				return false;
			}

			const bool isSucceeded = (0 != WideCharToMultiByte(CP_ACP, 0, origin, -1, output, outputLen, NULL, NULL));

			return isSucceeded;
		}

		return false;
#else
		return (0 == strcpy_s(output, outputMaxLen, origin));
#endif
	}
}

#define FormatString(Output, Format, ...)\
{\
	TCHAR data[MAX_LOG_SIZE]{ 0, }; \
	Sprint(data, MAX_LOG_SIZE, TEXT(Format), ##__VA_ARGS__); \
	Output = data;\
}\

// 주석치고 구현해야하는 것들 확인
#define TA_TEMP_DEV(Message);

#ifdef TA_SERVER

// 가변인자 매크로 ... <=> __VA_ARGS__와 대응 // __VA_ARGS__앞에 ##를 붙이면 불필요한 쉼표가 자동으로 삭제 
#define TA_LOG_BASE(Head, Format, ...)\
{\
	TCHAR logData[MAX_LOG_SIZE]{0,};\
	TCHAR functionName[MAX_LOG_SIZE]{0,};\
	const bool isSucceeded = ta::CharToTChar(__FUNCTION__, functionName, MAX_LOG_SIZE);\
	if(true == isSucceeded)\
	{\
		Sprint(logData, MAX_LOG_SIZE, TEXT(Format), ##__VA_ARGS__); \
		PrintLog(TEXT("%s (%d) => %s => %s \n"), functionName, __LINE__, TEXT(Head), logData);\
	}\
}

#define TA_LOG_DEV(Format, ...) TA_LOG_BASE("[LOG]", Format, ##__VA_ARGS__)

#define TA_ASSERT_DEV(Expr, Format, ...)\
{\
	if (false == Expr)\
	{\
		TA_LOG_BASE("[ASSERT] expr : \"#Expr\"", Format, ##__VA_ARGS__);\
		DebugBreak();\
	}\
}


#define TA_COMPILE_DEV(Expr, Message)  typedef char msg[(Expr) ? 1 : -1];


#else




//#define TA_LOG_BASE(head, Format, ...)
//#define TA_LOG_DEV(Format, ...) TA_LOG_BASE("[LOG]", Format, ##__VA_ARGS__)
//#define TA_ASSERT_DEV(Expr, Format, ...)
//#define TA_COMPILE_DEV(Expr, Message)  typedef char msg[(Expr) ? 1 : -1];

#define TA_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
// 가변인자 매크로 ... <=> __VA_ARGS__와 대응 // __VA_ARGS__앞에 ##를 붙이면 불필요한 쉼표가 자동으로 삭제 
#define TA_LOG_BASE(Verbosity, Format, ...) UE_LOG(TAGameClient, Verbosity, TEXT("%s => %s"), *TA_LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define TA_LOG_DEV(Format, ...) TA_LOG_BASE(Warning, "[LOG] DESC : %s", *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define TA_ASSERT_DEV(Expr, Format, ...) {if (!(Expr)){TA_LOG_BASE(Error, "[ASSERTION] EXPR : %s , DESC : %s", TEXT("'"#Expr"'"), *FString::Printf(TEXT(Format), ##__VA_ARGS__));}}
#define TA_ASSERTR_DEV(Expr, Format, ...) {if (!(Expr)){TA_LOG_BASE(Error, "[ASSERTION] EXPR : %s , DESC : %s", TEXT("'"#Expr"'"), *FString::Printf(TEXT(Format), ##__VA_ARGS__));return __VA_ARGS__;}}
#define TA_COMPILE_DEV(Expr, Message)  typedef char msg[(Expr) ? 1 : -1];

#endif


