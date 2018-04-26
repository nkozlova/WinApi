#pragma once
#include <Windows.h>

class CEllipseWindow {
public:
	CEllipseWindow();
	~CEllipseWindow();

	// ���������������� ����� ����
	static bool	RegisterClass_();

	// ������� ��������� ����
	bool Create(HWND hWndParent);

	// �������� ����
	void Show(int cmdShow);

	// ����� ����� ����
	HWND GetWindowHandle() { return handle; }

protected:
	void OnCreate();	// �������� ��������� WM_CREATE � �������� ����

	void OnNCCreate(const HWND handle);	// �������� ��������� WM_NCCREATE � �������� ��������� ����

	void OnDestroy();	// �������� ��������� WM_DESTROY

	void OnPaint();	// ������� ��������� WM_PAINT

	void OnTimer();	// �������� ��������� WM_TIMER
					
	void OnLButtonDown();	// �������� ��������� WM_LBUTTONDOWN

private:
	HWND handle;  // ����� ����
	UINT_PTR timerId; // Id �������, ������� �������� �� ���������� ��������
	int currentTime; // ������� �����
	bool flag;

	// ������ ��������, ������� ���� ����� ���������� (������ � ����������� �����)
	void makeEllipse(HDC hdc, int left, int top, int right, int bottom);

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};