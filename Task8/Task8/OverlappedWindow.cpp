#include "OverlappedWindow.h"

#define CHILD_COUNT_IN_WIDTH 4
#define CHILD_COUNT_IN_HEIGHT 3
#define CHILD_COUNT CHILD_COUNT_IN_WIDTH * CHILD_COUNT_IN_HEIGHT

COverlappedWindow::COverlappedWindow()
{
	for (int childId = 0; childId < CHILD_COUNT_IN_WIDTH * CHILD_COUNT_IN_HEIGHT; childId++) {
		childWindows.push_back(CEllipseWindow());
	}
}

COverlappedWindow::~COverlappedWindow() {}

bool COverlappedWindow::RegisterClass_()
{
	WNDCLASSEX windowClass;

	// Заполним структуру соответствующими данными
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = COverlappedWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = "COverlappedWindow";

	// Регестрируем класс окна
	return RegisterClassEx(&windowClass); 
}

bool COverlappedWindow::Create()
{
	CreateWindowEx(NULL,
		"COverlappedWindow",
		"Big window",
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

	// Показываем дочерние окна
	for (int childId = 0; childId < CHILD_COUNT; childId++) {
		childWindows[childId].Show(cmdShow);
	}
}

void COverlappedWindow::OnCreate()
{
	CEllipseWindow::RegisterClass_();	// Зарегистрируем класс окна CEditControlWindow

	// Создаем дочерние окна
	for (int childId = 0; childId < CHILD_COUNT; childId++) {
		childWindows[childId].Create(handle);
	}
}

void COverlappedWindow::OnNCCreate(HWND _handle)
{
	handle = _handle;
}

void COverlappedWindow::OnDestroy()
{
	PostQuitMessage(0);
}

void COverlappedWindow::OnSize()
{
	RECT clientRect;
	GetClientRect(handle, &clientRect);

	// Ширина и высота каждого из дочерних окон
	int childWidth = (clientRect.right - clientRect.left) / CHILD_COUNT_IN_WIDTH;
	int childHeight = (clientRect.bottom - clientRect.top) / CHILD_COUNT_IN_HEIGHT;

	// Установим местоположение каждого из дочерних окон
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		SetWindowPos(childWindows[i].GetWindowHandle(),
				HWND_TOP, clientRect.left + (i % CHILD_COUNT_IN_WIDTH) * childWidth,
			clientRect.top + (i / CHILD_COUNT_IN_WIDTH) * childHeight, childWidth, childHeight, NULL);
	}
}

LRESULT COverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	COverlappedWindow* window = reinterpret_cast<COverlappedWindow*>(GetWindowLongPtr(handle, GWLP_HINSTANCE));
	switch (message) {
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
		case WM_SIZE:
		{
			window->OnSize();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_DESTROY:
		{
			window->OnDestroy();
			return 0;
		}
		default:
			return DefWindowProc(handle, message, wParam, lParam);
	}
}