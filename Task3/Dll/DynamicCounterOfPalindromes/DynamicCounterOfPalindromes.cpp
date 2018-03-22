#include <windows.h>
#include <string>
#include <iostream>
#include <exception>
#include "../Dll/PalindromeCounter.h"
#include "../Utils/Utils.h"


/*
Сделайте вторую версию exe-модуля, в котором функцию PalindromeCounter вызывайте динамически.
*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {

	try {
		std::wstring path = GetFilePathFromArgs();
		wchar_t* text = ReadTextFile(path);
		ShowCountOfPalindromes(PalindromeCounter(text));	// Вывод результата подсчета количества палиндромов в тексте
	}
	catch (const std::exception &a) {
	}
	
	return 0;
}