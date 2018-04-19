#include <Windows.h>
#include <atlstr.h>
#include <cstdlib>

#include "OverlappedWindow.h"

void PrintError(const char* errorText)
{
	int lastError = static_cast<int>(GetLastError());
	char errorString[10];
	_itoa_s(lastError, errorString, 10);

	MessageBox(NULL,
		CString(errorString),
		CString(errorText),
		MB_DEFBUTTON1 | //Выделеная кнопка
		MB_ICONEXCLAMATION | //Тип иконки
		MB_DEFAULT_DESKTOP_ONLY);
}

void PrintMessage(const char* messageText) {
	MessageBox(NULL,
		CString(messageText),
		CString("Success"),
		MB_DEFBUTTON1 | //Выделеная кнопка
		MB_DEFAULT_DESKTOP_ONLY);
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow)
{
	COverlappedWindow myWindow;
	bool registrationSuccess = myWindow.RegisterClass_();

	if (!registrationSuccess) {
		PrintError("Registration failed\nError:");
		exit(-1);
	}
	//PrintMessage("Registration");

	bool creatingSuccess = myWindow.Create();

	if (!creatingSuccess) {
		PrintError("Creating failed\nError:");
		exit(-1);
	}
	//PrintMessage("Creating");

	myWindow.Show(cmdShow);

	// Запускаем цикл обработки сообщений
	MSG message;
	BOOL getMessageSucceed = 0;
	while ((getMessageSucceed = GetMessage(&message, 0, 0, 0)) != 0) {
		if (getMessageSucceed == -1) {
			exit(-1);
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	PrintMessage("All");
	
	return 0;
}