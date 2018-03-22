// ExePalindromeCounter.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include <windows.h>
#include <string>
#include <iostream>
#include <exception>
#include "../Utils/Utils.h"


static const std::string PALINDROME_COUNTER_LIB = "Dll.dll";
static const std::string PALINDROME_COUNTER_PROC_NAME = "PalindromeCounter";

typedef int( *PalindromeCounterPtr )( const wchar_t * );

// ������� �������� ����������
HMODULE LoadDll(const std::string &path) {
	HMODULE dllHandle = LoadLibrary((LPCSTR)path.c_str());
	if (dllHandle == 0) {
		throw std::runtime_error("Fail to load dll");
	}
	return dllHandle;
}

// ����� �������, ����������� � dll
int CallPalindromeCounter(HMODULE dllHanlde, const std::wstring &text) {
	FARPROC countOfPalindromesProc = GetProcAddress(dllHanlde, PALINDROME_COUNTER_PROC_NAME.c_str());
	if (countOfPalindromesProc == 0) {
		throw std::runtime_error("Palindromes Count function is not found in the library");
	}
	return ((PalindromeCounterPtr)(countOfPalindromesProc))(text.c_str());
}


/*
���������� exe-������, ����������� � �������� ��������� �� ��������� ������ ���� � ����� � �������. 
��������� ������ ������� ������������� ������� PalindromeCounter. 
����������� ���������� ���� ����������� � MessageBox.
*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {

	try {
		std::wstring path = GetFilePathFromArgs();	// ���� �� ����� ������ � �������������� ���������� ��������� ������
		wchar_t* text = ReadTextFile(path);
		HMODULE dllHandle = LoadDll(PALINDROME_COUNTER_LIB);
		ShowCountOfPalindromes(CallPalindromeCounter(dllHandle, text));	// ����� � MessageBox ���������� �������
		FreeLibrary(dllHandle);		// ������������ ����������
	}
	catch (const std::exception &a) {
	}

	return 0;
}