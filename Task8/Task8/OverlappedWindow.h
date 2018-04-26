#pragma once
#include <Windows.h>
#include <vector>

#include "EllipseWindow.h"

class COverlappedWindow {
public:
	COverlappedWindow();
	~COverlappedWindow();

	// Зарегистрировать класс окна
	static bool	RegisterClass_();

	// Создать экземпляр окна
	bool Create();

	// Показать окно
	void Show(int cmdShow);

	// Взять хэндл окна
	HWND GetWindowHandle() { return handle; }

protected:
	void OnCreate();	// Получаем сообщение WM_CREATE о создании окна

	void OnNCCreate(const HWND handle);	// Получаем сообщение WM_NCCREATE о создании контекста окна

	void OnDestroy();	// Получаем сообщение WM_DESTROY

	void OnSize();	// Получаем сообщение WM_SIZE

private:
	HWND handle;  // хэндл окна
	std::vector<CEllipseWindow> childWindows; // дочерние дети окна

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};