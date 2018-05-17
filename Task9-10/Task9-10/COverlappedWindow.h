#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <vector>
#include <memory>

#include "CEditControlWindow.h"
#include "CDialogWindow.h"
#include "resource1.h"


class COverlappedWindow {
public:
	COverlappedWindow();
	~COverlappedWindow();

	// Зарегистрировать класс окна
	static bool	RegisterClass();

	// Создать экземпляр окна
	bool Create();

	// Показать окно
	void Show(int cmdShow);

	// Вернуть хендл окна
	HWND GetWindowHandle() { return handle; }

	// Вернуть хендл child-edit-control окна
	HWND GetEditControlHandle() { return childEditControlWindow.GetWindowHandle(); }

	// Вернуть хендл dialog окна
	HWND GetDialogHandle() { return dialogWindow.GetWindowHandle(); }

	// Вернуть цвет фона
	COLORREF GetBackgroundColor() { return backgroundColor; }

	// Вернуть цвет шрифта
	COLORREF GetFontColor() { return fontColor; }

	// Вернуть прозрачность
	BYTE GetTransparency() { return transparency; }

	// Установить новый цвет фона
	void SetBackgroundColor(COLORREF newBackgroundColor) { backgroundColor = newBackgroundColor; }

	// Устаносить новый цвет текста
	void SetFontColor(COLORREF newFontColor) { fontColor = newFontColor; }

	// Установить новую прозрачность
	void SetTransparency(BYTE newTransparency) { transparency = newTransparency; }

protected:
	// Вызывается при получении сообщения WM_CREATE (создание окна)
	void OnCreate();

	// Вызывается при получении сообщения WM_NCCREATE (создание контекста окна)
	void OnNCCreate(HWND handle);

	// Вызывается при получении сообщения WM_SIZE (об изменении размеров окна)
	void OnSize();

	// Вызывается при получении сообщения WM_COMMAND
	void OnCommand(WPARAM wParam, LPARAM lParam);

	// Вызывается, когда получена команда меню
	void OnCommandMenu(WPARAM wParam);

	// Вызывается при получении сообщения WM_CLOSE (закрытие окна)
	// Возвращает true если окно нужно закрыть и false иначе
	bool OnClose();

	// Вызывается при получении сообщения WM_DESTROY
	void OnDestroy();

	LRESULT OnControlColorEdit(HDC hdc);

private:
	HWND handle;  // хэндл окна
	CEditControlWindow childEditControlWindow; // дочернее edit-control окно, занимает всю клиентскую область
	CDialogWindow dialogWindow; // окно, которое появляется при выборе в меню View-Settings

	bool textWasEntered; // флаг, показывающий, был ли введён текст в edit-control окно
	HMENU windowMenu; // хэндл меню окна

					  // Настраиваемые элементы - цвета, размеры и пр.
	COLORREF backgroundColor; // Цвет фона
	COLORREF fontColor; // Цвет шрифта
	BYTE transparency; // Прозрачность окна

	HBRUSH activeBrush;

	// Метод сохранения текста
	void saveText();

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};