#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <string>

#ifndef ASSERT
#define ASSERT(x) if(!(x)) __debugbreak()
#endif // !ASSERT

#ifndef SAFE_DELETE_HANDLE
#define SAFE_DELETE_HANDLE(x) if(x) CloseHandle(x)
#endif // !SAFE_DELETE_HANDLE

void LogWinsockError(std::string errorMsg, int winsockErrorCode);

template<typename T>
bool CompareFloatingPoint(
	T lhs, T rhs, T precision
);


template<>
bool CompareFloatingPoint(
	float lhs, float rhs, float precision
);

template<>
bool CompareFloatingPoint(
	double lhs, double rhs, double precision
);


std::vector<std::string> SplitString(
	std::string str, char delimiter
);

void LTrimString(std::string & str);
void RTrimString(std::string & str);
void TrimString(std::string & str);
inline bool IsWhiteSpace(std::string str);

std::wstring CurrentDirectoryPath();
