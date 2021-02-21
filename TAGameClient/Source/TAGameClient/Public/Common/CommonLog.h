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

typedef std::basic_string<TCHAR> tstring;

//#endif

namespace ta
{
	static bool CharToTChar(const char* origin, TCHAR* output)
	{
#ifdef UNICODE
		if (nullptr != origin && nullptr != output)
		{
			const int originLen = (int)strlen(origin);
			const int outputLen = MultiByteToWideChar(CP_ACP, 0, origin, originLen, NULL, NULL);
			const bool isSucceeded = (0 != MultiByteToWideChar(CP_ACP, 0, origin, originLen, output, outputLen));

			return isSucceeded;
		}
#endif
		return true;
	}
}

#ifdef TA_SERVER

// 가변인자 매크로 ... <=> __VA_ARGS__와 대응 // __VA_ARGS__앞에 ##를 붙이면 불필요한 쉼표가 자동으로 삭제 
#define TA_LOG_BASE(head, Format, ...)\
{\
	TCHAR logData[MAX_LOG_SIZE]{0,};\
	TCHAR functionName[MAX_LOG_SIZE]{0,};\
	const bool isSucceeded = ta::CharToTChar(__FUNCTION__, functionName);\
	if(true == isSucceeded)\
	{\
		Sprint(logData, MAX_LOG_SIZE, TEXT(Format), ##__VA_ARGS__); \
		PrintLog(TEXT("%s (%d) => %s => %s \n"), functionName, __LINE__, TEXT(head), logData);\
	}\
}

#define TA_LOG_DEV(Format, ...) TA_LOG_BASE("[LOG]", Format, ##__VA_ARGS__)

#define TA_ASSERT_DEV(Expr, Format, ...)\
{\
	if (false == Expr)\
	{\
		TA_LOG_BASE("[ASSERT] expr : "#Expr"", Format, ##__VA_ARGS__);\
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


