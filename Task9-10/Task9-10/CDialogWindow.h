#pragma once
#include <Windows.h>
#include "resource1.h"


// �����, ���������� � ���� ���������� � ���������� �����������
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

	// ������� � �������� ��������� ����
	bool Create(HWND hWndParent);

	// ������� ����� ����
	HWND GetWindowHandle() { return handle; }

protected:
	// ���������� ��� ��������� ��������� WM_INITDIALOG
	void OnInit(HWND _handle);

	// ���������� ��� ��������� ��������� WM_COMMAND
	INT_PTR OnCommand(WPARAM wParam);

	// ���������� ��� ��������� ��������� WM_HSCROLL
	void OnScroll(WPARAM wParam, LPARAM lParam);

	// ���������� ��� ��������� ��������� WM_CLOSE
	void OnClose(WPARAM wParam);

private:
	HWND handle;  // ����� ����

	bool isPreview; // ����, ������������, ��� ���������� ����� �������������
	CDisplaySettings setSettings; // ������������� ��������
	CDisplaySettings newSettings; // ��������, ������� ����� ���� �����������

	HWND overlappedWindowHandle;
	HWND editControlHandle;

	HFONT newFont;

	void changeSettingsTo(const CDisplaySettings &settings);

	void setNewColorTo(DWORD &whatToChange);

	static INT_PTR _stdcall dialogProc(HWND handleDlg, UINT message, WPARAM wParam, LPARAM lParam);
};