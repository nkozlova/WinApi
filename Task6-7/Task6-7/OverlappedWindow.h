#pragma once
#include <Windows.h>

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

protected:
	void OnCreate();	// �������� ��������� WM_CREATE � �������� ����
	
	void OnNCCreate(const HWND handle);	// �������� ��������� WM_NCCREATE � �������� ��������� ����

	void OnDestroy();	// �������� ��������� WM_DESTROY

	void OnPaint();	// ������� ��������� WM_PAINT

	void OnTimer();	// �������� ��������� WM_TIMER
	
private:
	HWND handle;  // ����� ����
	UINT_PTR timerId; // Id �������, ����������� �� ���������� ��������
	int currentTime; // ������� �����

	void makeEllipse(HDC hdc, int a, int b, int c, int d);	// �������� �������

	static LRESULT _stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};