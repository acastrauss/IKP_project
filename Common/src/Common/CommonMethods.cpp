#include "pch.h"
#include "Common/CommonMethods.h"
#include <iostream>
#include <sstream>
#include <algorithm>

template<typename T>
bool CompareFloatingPoint(T lhs, T rhs, T precision)
{
	static_assert(false);
}

template<>
bool CompareFloatingPoint(float lhs, float rhs, float precision)
{
	return
		abs(lhs - rhs) < precision;
}

template<>
bool CompareFloatingPoint(double lhs, double rhs, double precision)
{
	return
		abs(lhs - rhs) < precision;
}

std::vector<std::string> SplitString(std::string str, char delimiter)
{
	std::vector<std::string> result = {};
	std::stringstream ss(str);
	std::string item;
	
	while (std::getline(ss, item, delimiter)) {
		result.push_back(item);
	}

	return result;
}

void LTrimString(std::string& str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

void RTrimString(std::string& str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), str.end());
}

void TrimString(std::string& str)
{
	[&](std::string& str) {
		LTrimString(str);
		RTrimString(str);
	}(str);
}

inline bool IsWhiteSpace(std::string str)
{
	return std::all_of(str.begin(), str.end(), std::isspace);
}

std::wstring CurrentDirectoryPath()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}