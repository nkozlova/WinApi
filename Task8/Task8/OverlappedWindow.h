#pragma once
#include <Windows.h>
#include <vector>

#include "EllipseWindow.h"

class COverlappedWindow {
public:
	COverlappedWindow();
	~COverlappedWindow();

	// ���������������� ����� ����
	static bool	RegisterClass_();

	// ������� ��������� ����
	bool Create();

	// �������� ����
	void Show(int cmdShow);

	// ����� ����� ����
	HWND GetWindowHandle() { return handle; }

protected:
	void OnCreate();	// �������� ��������� WM_CREATE � �������� ����

	void OnNCCreate(const HWND handle);	// �������� ��������� WM_NCCREATE � �������� ��������� ����

	void OnDestroy();	// �������� ��������� WM_DESTROY

	void OnSize();	// �������� ��������� WM_SIZE

private:
	HWND handle;  // ����� ����
	std::vector<CEllipseWindow> childWindows; // �������� ���� ����

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};