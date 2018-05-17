#include <Windows.h>
#include <atlstr.h> // CString
#include <cstdlib>

#include "COverlappedWindow.h"


void PrintError(const char* error)
{
	int lastError = static_cast<int>(GetLastError());
	char errorString[10];
	_itoa_s(lastError, errorString, 10);

	MessageBox(NULL,
		CString(errorString),
		CString(error),
		MB_DEFBUTTON1 | // Выделенная кнопка
		MB_ICONEXCLAMATION | //Тип ошибки
		MB_DEFAULT_DESKTOP_ONLY);
}


int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow)
{
	COverlappedWindow myWindow;
	bool registrationSuccess = myWindow.RegisterClass();

	if (!registrationSuccess) {
		PrintError("Could't registrate window! Error code:");
		exit(-1);
	}

	bool creatingSuccess = myWindow.Create();

	if (!creatingSuccess) {
		PrintError("Couln't create window! Error code:");
		exit(-1);
	}

	myWindow.Show(cmdShow);

	MSG message;
	BOOL getMessageSucceed = 0;
	while ((getMessageSucceed = GetMessage(&message, 0, 0, 0)) != 0) {
		if (!IsDialogMessage(myWindow.GetDialogHandle(), &message)) {
			if (getMessageSucceed == -1) {
				exit(-1);
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	return 0;
}
