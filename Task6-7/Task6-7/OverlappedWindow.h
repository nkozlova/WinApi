#pragma once
#include <Windows.h>

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

protected:
	void OnCreate();	// Получаем сообщение WM_CREATE о создании окна
	
	void OnNCCreate(const HWND handle);	// Получаем сообщение WM_NCCREATE о создании контекста окна

	void OnDestroy();	// Получаем сообщение WM_DESTROY

	void OnPaint();	// Получем сообщение WM_PAINT

	void OnTimer();	// Получаем сообщение WM_TIMER
	
private:
	HWND handle;  // хэндл окна
	UINT_PTR timerId; // Id таймера, сообщающего об обновлении анимации
	int currentTime; // Текущее време

	void makeEllipse(HDC hdc, int a, int b, int c, int d);	// Создание эллипса

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};