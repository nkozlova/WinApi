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

	// ���������������� ����� ����
	static bool	RegisterClass();

	// ������� ��������� ����
	bool Create();

	// �������� ����
	void Show(int cmdShow);

	// ������� ����� ����
	HWND GetWindowHandle() { return handle; }

	// ������� ����� child-edit-control ����
	HWND GetEditControlHandle() { return childEditControlWindow.GetWindowHandle(); }

	// ������� ����� dialog ����
	HWND GetDialogHandle() { return dialogWindow.GetWindowHandle(); }

	// ������� ���� ����
	COLORREF GetBackgroundColor() { return backgroundColor; }

	// ������� ���� ������
	COLORREF GetFontColor() { return fontColor; }

	// ������� ������������
	BYTE GetTransparency() { return transparency; }

	// ���������� ����� ���� ����
	void SetBackgroundColor(COLORREF newBackgroundColor) { backgroundColor = newBackgroundColor; }

	// ���������� ����� ���� ������
	void SetFontColor(COLORREF newFontColor) { fontColor = newFontColor; }

	// ���������� ����� ������������
	void SetTransparency(BYTE newTransparency) { transparency = newTransparency; }

protected:
	// ���������� ��� ��������� ��������� WM_CREATE (�������� ����)
	void OnCreate();

	// ���������� ��� ��������� ��������� WM_NCCREATE (�������� ��������� ����)
	void OnNCCreate(HWND handle);

	// ���������� ��� ��������� ��������� WM_SIZE (�� ��������� �������� ����)
	void OnSize();

	// ���������� ��� ��������� ��������� WM_COMMAND
	void OnCommand(WPARAM wParam, LPARAM lParam);

	// ����������, ����� �������� ������� ����
	void OnCommandMenu(WPARAM wParam);

	// ���������� ��� ��������� ��������� WM_CLOSE (�������� ����)
	// ���������� true ���� ���� ����� ������� � false �����
	bool OnClose();

	// ���������� ��� ��������� ��������� WM_DESTROY
	void OnDestroy();

	LRESULT OnControlColorEdit(HDC hdc);

private:
	HWND handle;  // ����� ����
	CEditControlWindow childEditControlWindow; // �������� edit-control ����, �������� ��� ���������� �������
	CDialogWindow dialogWindow; // ����, ������� ���������� ��� ������ � ���� View-Settings

	bool textWasEntered; // ����, ������������, ��� �� ����� ����� � edit-control ����
	HMENU windowMenu; // ����� ���� ����

					  // ������������� �������� - �����, ������� � ��.
	COLORREF backgroundColor; // ���� ����
	COLORREF fontColor; // ���� ������
	BYTE transparency; // ������������ ����

	HBRUSH activeBrush;

	// ����� ���������� ������
	void saveText();

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};