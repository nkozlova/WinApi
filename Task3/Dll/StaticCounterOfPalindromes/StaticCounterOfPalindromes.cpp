#include <windows.h>
#include <string>
#include <iostream>
#include <exception>
#include "../StaticLib/PalindromeCounter.h"
#include "../Utils/Utils.h"


/*
�������� ������ ������ exe-������, ������������ �� ����������� �����������
*/
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{

	try {
		std::wstring path = GetFilePathFromArgs();
		wchar_t* text = ReadTextFile(path);
		ShowCountOfPalindromes(PalindromeCounter(text));	// ����� ���������� �������� ���������� �����
	}
	catch (const std::exception &a) {
	}

	return 0;
}