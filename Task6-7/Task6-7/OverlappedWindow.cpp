#include "OverlappedWindow.h"

COverlappedWindow::COverlappedWindow()
{
	currentTime = 0;
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
	windowClass.lpszClassName = "СOverlappedWindow";

	// Регестрируем класс окна
	return RegisterClassEx(&windowClass);
}

bool COverlappedWindow::Create()
{
	CreateWindowEx(NULL,
		"СOverlappedWindow",
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
	PostQuitMessage(0);	// Указываем системе запрос о завершении
}

void COverlappedWindow::OnPaint()
{
	SetWindowPos(handle, HWND_TOP, 100, 100, 600, 250, SWP_SHOWWINDOW);	// Фиксируем размер фона

	PAINTSTRUCT structToPaint;
	HDC displayHandle = BeginPaint(handle, &structToPaint);

	RECT clientRect;  // Структура, в которой будут координаты клиентской области
	GetClientRect(handle, &clientRect);	// Получаем координаты клиентской области
	
	HBRUSH backgroundBrush = CreateSolidBrush(RGB(100, 0, 250));	// Кисть для закрашивания фона
	SelectObject(displayHandle, backgroundBrush);	// Делаем кисть для закрашивания фона активной
	FillRect(displayHandle, &clientRect, backgroundBrush);	// Красим фон
	
	// Обеспечение плавной отрисовки
	HDC bufferDC = CreateCompatibleDC(displayHandle);	// Создаем контекст памяти -- поверхность отображения
	HBITMAP bufferBitmap = CreateCompatibleBitmap(displayHandle,
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top);	// Создаем соответствующую битовую карту
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
		displayHandle, 0, 0, SRCCOPY);	// Битово передаем цветовые данные

	// Удаляем использованные объекты
	DeleteObject(backgroundBrush);
	DeleteDC(bufferDC);
	DeleteObject(bufferBitmap);

	EndPaint(handle, &structToPaint);
}

void COverlappedWindow::OnTimer()
{
	RECT clientRect;
	GetClientRect(handle, &clientRect);	// Получаем прямоугольник клиентской области
	currentTime += 1;
	InvalidateRect(handle, &clientRect, TRUE);	// Прямоугольник для перерисовки окна
}

void COverlappedWindow::makeEllipse(HDC hdc, int a, int b, int c, int d)
{
	HBRUSH insideEllipsBrush = CreateSolidBrush(RGB(255, 0, 255));	// Кисть для рисования эллипса
	SelectObject(hdc, insideEllipsBrush);	// Делаем кисть для рисования эллипса активной
	
	Ellipse(hdc, a, b, c, d);

	DeleteObject(insideEllipsBrush);	// Заканчиваем использование кисти
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
