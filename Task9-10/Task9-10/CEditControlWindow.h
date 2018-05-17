#pragma once
#include <Windows.h>

class CEditControlWindow {
public:
	CEditControlWindow();
	~CEditControlWindow();

	// Создать экземпляр окна
	bool Create(HWND hWndParent);

	// Показать окно
	void Show(int cmdShow);

	// Вернуть хендл окна
	HWND GetWindowHandle() { return handle; }

private:
	HWND handle;  // хэндл окна
};