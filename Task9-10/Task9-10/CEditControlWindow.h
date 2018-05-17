#pragma once
#include <Windows.h>

class CEditControlWindow {
public:
	CEditControlWindow();
	~CEditControlWindow();

	// ������� ��������� ����
	bool Create(HWND hWndParent);

	// �������� ����
	void Show(int cmdShow);

	// ������� ����� ����
	HWND GetWindowHandle() { return handle; }

private:
	HWND handle;  // ����� ����
};