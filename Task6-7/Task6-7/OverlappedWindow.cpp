#include "OverlappedWindow.h"

COverlappedWindow::COverlappedWindow()
{
	currentTime = 0;
}

COverlappedWindow::~COverlappedWindow() {}

bool COverlappedWindow::RegisterClass_()
{
	WNDCLASSEX windowClass;

	// �������� ��������� ���������������� �������
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = COverlappedWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = "�OverlappedWindow";

	// ������������ ����� ����
	return RegisterClassEx(&windowClass);
}

bool COverlappedWindow::Create()
{
	CreateWindowEx(NULL,
		"�OverlappedWindow",
		"My window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this);

	return handle != NULL;
}

void COverlappedWindow::Show(int cmdShow)
{
	ShowWindow(handle, cmdShow);
}

void COverlappedWindow::OnCreate()
{
	const int timer = 100;
	timerId = SetTimer(handle, NULL, timer, NULL);
}

void COverlappedWindow::OnNCCreate(const HWND _handle)
{
	handle = _handle;
}

void COverlappedWindow::OnDestroy()
{
	KillTimer(handle, timerId);
	PostQuitMessage(0);	// ��������� ������� ������ � ����������
}

void COverlappedWindow::OnPaint()
{
	SetWindowPos(handle, HWND_TOP, 100, 100, 600, 250, SWP_SHOWWINDOW);	// ��������� ������ ����

	PAINTSTRUCT structToPaint;
	HDC displayHandle = BeginPaint(handle, &structToPaint);

	RECT clientRect;  // ���������, � ������� ����� ���������� ���������� �������
	GetClientRect(handle, &clientRect);	// �������� ���������� ���������� �������
	
	HBRUSH backgroundBrush = CreateSolidBrush(RGB(100, 0, 250));	// ����� ��� ������������ ����
	SelectObject(displayHandle, backgroundBrush);	// ������ ����� ��� ������������ ���� ��������
	FillRect(displayHandle, &clientRect, backgroundBrush);	// ������ ���
	
	// ����������� ������� ���������
	HDC bufferDC = CreateCompatibleDC(displayHandle);	// ������� �������� ������ -- ����������� �����������
	HBITMAP bufferBitmap = CreateCompatibleBitmap(displayHandle,
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top);	// ������� ��������������� ������� �����
	SelectObject(bufferDC, bufferBitmap);

	if (currentTime % 100 < 50) {
		makeEllipse(displayHandle,
			clientRect.left + currentTime % 100,
			clientRect.top + currentTime % 100,
			clientRect.right - clientRect.left - 50 + currentTime % 100,
			clientRect.bottom - clientRect.top - 50 + currentTime % 100);
	}
	else {
		makeEllipse(displayHandle,
			clientRect.left - currentTime % 100 + 100,
			clientRect.top - currentTime % 100 + 100,
			clientRect.right - clientRect.left - 50 - currentTime % 100 + 100,
			clientRect.bottom - clientRect.top - 50 - currentTime % 100 + 100);
	}

	BitBlt(displayHandle, clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 
		displayHandle, 0, 0, SRCCOPY);	// ������ �������� �������� ������

	// ������� �������������� �������
	DeleteObject(backgroundBrush);
	DeleteDC(bufferDC);
	DeleteObject(bufferBitmap);

	EndPaint(handle, &structToPaint);
}

void COverlappedWindow::OnTimer()
{
	RECT clientRect;
	GetClientRect(handle, &clientRect);	// �������� ������������� ���������� �������
	currentTime += 1;
	InvalidateRect(handle, &clientRect, TRUE);	// ������������� ��� ����������� ����
}

void COverlappedWindow::makeEllipse(HDC hdc, int a, int b, int c, int d)
{
	HBRUSH insideEllipsBrush = CreateSolidBrush(RGB(255, 0, 255));	// ����� ��� ��������� �������
	SelectObject(hdc, insideEllipsBrush);	// ������ ����� ��� ��������� ������� ��������
	
	Ellipse(hdc, a, b, c, d);

	DeleteObject(insideEllipsBrush);	// ����������� ������������� �����
}

LRESULT COverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(GetWindowLongPtr(handle, GWLP_HINSTANCE));

	switch (message) 
	{
		case WM_CREATE:
		{
			window->OnCreate();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_NCCREATE:
		{
			window = reinterpret_cast<COverlappedWindow*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
			SetWindowLongPtr(handle, GWLP_HINSTANCE, (LONG)window);
			window->OnNCCreate(handle);
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_DESTROY:
		{
			window->OnDestroy();
			return 0;
		}
		case WM_PAINT:
		{
			window->OnPaint();
			return 0;
		}
		case WM_TIMER:
		{
			window->OnTimer();
			return 0;
		}
		default:
			return DefWindowProc(handle, message, wParam, lParam);
	}
}
