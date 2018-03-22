#pragma once

#include <string>


std::wstring GetFilePathFromArgs();
wchar_t* ReadTextFile(const std::wstring &path);
void ShowCountOfPalindromes(int countOfPalindromes);
void ShowError(const std::string &message);


