#include <windows.h>
#include <string>
#include <iostream>
#include <exception>
#include "../StaticLib/PalindromeCounter.h"
#include "../Utils/Utils.h"


/*
Ќапишите третью версию exe-модул€, линкующегос€ со статической библиотекой
*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{

	try {
		std::wstring path = GetFilePathFromArgs();
		wchar_t* text = ReadTextFile(path);
		ShowCountOfPalindromes(PalindromeCounter(text));	// ¬ывод результата подсчета количества строк
	}
	catch (const std::exception &a) {
	}

	return 0;
}