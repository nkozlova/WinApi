#pragma once
#include <Windows.h>

class CEllipseWindow {
public:
	CEllipseWindow();
	~CEllipseWindow();

	// Зарегистрировать класс окна
	static bool	RegisterClass_();

	// Создать экземпляр окна
	bool Create(HWND hWndParent);

	// Показать окно
	void Show(int cmdShow);

	// Взять хэндл окна
	HWND GetWindowHandle() { return handle; }

protected:
	void OnCreate();	// Получаем сообщение WM_CREATE о создании окна

	void OnNCCreate(const HWND handle);	// Получаем сообщение WM_NCCREATE о создании контекста окна

	void OnDestroy();	// Получаем сообщение WM_DESTROY

	void OnPaint();	// Получем сообщение WM_PAINT

	void OnTimer();	// Получаем сообщение WM_TIMER
					
	void OnLButtonDown();	// Получаем сообщение WM_LBUTTONDOWN

private:
	HWND handle;  // Хэндл окна
	UINT_PTR timerId; // Id таймера, который сообщает об обновлении анимации
	int currentTime; // Текущее время
	bool flag;

	// Создаёт картинку, которую надо будет нарисовать (эллипс в определённой точке)
	void makeEllipse(HDC hdc, int left, int top, int right, int bottom);

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};