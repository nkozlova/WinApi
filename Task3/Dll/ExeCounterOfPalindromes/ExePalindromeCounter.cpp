// ExePalindromeCounter.cpp: определяет точку входа для консольного приложения.
//

#include <windows.h>
#include <string>
#include <iostream>
#include <exception>
#include "../Utils/Utils.h"


static const std::string PALINDROME_COUNTER_LIB = "Dll.dll";
static const std::string PALINDROME_COUNTER_PROC_NAME = "PalindromeCounter";

typedef int( *PalindromeCounterPtr )( const wchar_t * );

// Функция загрузки библиотеки
HMODULE LoadDll(const std::string &path) {
	HMODULE dllHandle = LoadLibrary((LPCSTR)path.c_str());
	if (dllHandle == 0) {
		throw std::runtime_error("Fail to load dll");
	}
	return dllHandle;
}

// Вызов функции, объявленной в dll
int CallPalindromeCounter(HMODULE dllHanlde, const std::wstring &text) {
	FARPROC countOfPalindromesProc = GetProcAddress(dllHanlde, PALINDROME_COUNTER_PROC_NAME.c_str());
	if (countOfPalindromesProc == 0) {
		throw std::runtime_error("Palindromes Count function is not found in the library");
	}
	return ((PalindromeCounterPtr)(countOfPalindromesProc))(text.c_str());
}


/*
Реализуйте exe-модуль, принимающий в качестве аргумента из командной строки путь к файлу с текстом. 
Программа должна вызвать импортируемую функцию PalindromeCounter. 
Вычисленное количество слов показывайте в MessageBox.
*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {

	try {
		std::wstring path = GetFilePathFromArgs();	// Путь до файла указан в дополнительных аргументах командной строки
		wchar_t* text = ReadTextFile(path);
		HMODULE dllHandle = LoadDll(PALINDROME_COUNTER_LIB);
		ShowCountOfPalindromes(CallPalindromeCounter(dllHandle, text));	// Вывод в MessageBox результата функции
		FreeLibrary(dllHandle);		// Освобождение библиотеки
	}
	catch (const std::exception &a) {
	}

	return 0;
}