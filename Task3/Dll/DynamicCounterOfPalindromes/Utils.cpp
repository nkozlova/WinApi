#include "Utils.h"
#include <string>
#include <Windows.h>
#include <cassert>
#include <fstream>
#include <wchar.h>

#define UNICODE


std::wstring GetFilePathFromArgs() {
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc < 2) {
		throw std::invalid_argument("There should be a path to unicode text file as argument");
	}
	else {
		return std::wstring(argv[1]);
	}
}

wchar_t* ReadTextFile(const std::wstring &path) {
	HANDLE fileHandle = CreateFileW(path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Can't open file");
	}

	DWORD fileSize = GetFileSize(fileHandle, &fileSize);
	size_t length = fileSize / sizeof(wchar_t);
	wchar_t* text = new wchar_t[length + 1];
	text[length] = L'\0';
	DWORD numberOfBytesRead;

	if (!ReadFile(fileHandle, text, fileSize, &numberOfBytesRead, 0) || numberOfBytesRead != fileSize) {
		CloseHandle(fileHandle);
		throw std::runtime_error("Fail to read file");
	}

	CloseHandle(fileHandle);

	return text;
}

void ShowCountOfPalindromes(int countOfPalindromes) {
	MessageBox(0, std::to_wstring(countOfPalindromes).c_str(), L"count of palindroms", MB_OK);
}
