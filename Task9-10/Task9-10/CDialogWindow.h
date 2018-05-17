#pragma once
#include <Windows.h>
#include "resource1.h"


// Класс, содержащий в себе информацию о настройках отображения
class CDisplaySettings {
public:
	DWORD backgroundColor;
	DWORD fontColor;
	LOGFONT font;
	BYTE transparency;
};

class CDialogWindow {
public:
	CDialogWindow();
	~CDialogWindow();

	// Создать и показать экземпляр окна
	bool Create(HWND hWndParent);

	// Вернуть хендл окна
	HWND GetWindowHandle() { return handle; }

protected:
	// Вызывается при получении сообщения WM_INITDIALOG
	void OnInit(HWND _handle);

	// Вызывается при получении сообщения WM_COMMAND
	INT_PTR OnCommand(WPARAM wParam);

	// Вызывается при получении сообщения WM_HSCROLL
	void OnScroll(WPARAM wParam, LPARAM lParam);

	// Вызывается при получении сообщения WM_CLOSE
	void OnClose(WPARAM wParam);

private:
	HWND handle;  // хэндл окна

	bool isPreview; // флаг, показывающий, что установлен режим предпросмотра
	CDisplaySettings setSettings; // установленные свойства
	CDisplaySettings newSettings; // свойства, которые могут быть установлены

	HWND overlappedWindowHandle;
	HWND editControlHandle;

	HFONT newFont;

	void changeSettingsTo(const CDisplaySettings &settings);

	void setNewColorTo(DWORD &whatToChange);

	static INT_PTR _stdcall dialogProc(HWND handleDlg, UINT message, WPARAM wParam, LPARAM lParam);
};