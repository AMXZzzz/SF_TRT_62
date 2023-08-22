#pragma once
#include <string>

std::wstring String2WString(const std::string& s);

std::string StringToUTF8(const std::string& str);

std::string UTF8ToAnsi(const char* strSrc);

 std::string GetIniPath();