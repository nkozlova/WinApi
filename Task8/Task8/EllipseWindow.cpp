#include "EllipseWindow.h"

CEllipseWindow::CEllipseWindow()
{
	currentTime = 0;
}

CEllipseWindow::~CEllipseWindow() {}

bool CEllipseWindow::RegisterClass_()
{
	WNDCLASSEX windowClass;

	// Заполним структуру соответствующими данными
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = CEllipseWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = "EllipseWindow";

	// Регестрируем класс окна
	return RegisterClassEx(&windowClass);
}

bool CEllipseWindow::Create(HWND hWndParent)
{
	CreateWindowEx(0,
		"EllipseWindow",
		"Small window",
		WS_CHILD | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWndParent,
		NULL,
		GetModuleHandle(NULL),
		this);

	return handle != NULL;
}

void CEllipseWindow::Show(int cmdShow)
{
	ShowWindow(handle, cmdShow);
}

void CEllipseWindow::OnCreate()
{
	const int timer = 100;
	timerId = SetTimer(handle, NULL, timer, NULL);
	flag = true;
}

void CEllipseWindow::OnNCCreate(HWND _handle)
{
	handle = _handle;
}

void CEllipseWindow::OnDestroy()
{
	KillTimer(handle, timerId);
	PostQuitMessage(0);	// Указываем системе запрос о завершении
}

void CEllipseWindow::OnPaint()
{
	PAINTSTRUCT structToPaint;
	HDC displayHandle = BeginPaint(handle, &structToPaint);

	RECT clientRect;  // Структура, в которой будут координаты клиентской области
	GetClientRect(handle, &clientRect);	// Получаем координаты клиентской области

	HBRUSH backgroundBrush = CreateSolidBrush(RGB(100, 0, 250));	// Кисть для закрашивания фона
	HGDIOBJ replacedBrush = SelectObject(displayHandle, backgroundBrush);	// Делаем кисть для закрашивания фона активной
	FillRect(displayHandle, &clientRect, backgroundBrush);	// Красим фон

	// Обеспечение плавной отрисовки
	HDC bufferDC = CreateCompatibleDC(displayHandle);	// Создаем контекст памяти -- поверхность отображения
	HBITMAP bufferBitmap = CreateCompatibleBitmap(displayHandle,
		clientRect.right - clientRect.left,
		clientRect.bottom - clientRect.top);	// Создаем соответствующую битовую карту
	SelectObject(bufferDC, bufferBitmap);

	HGDIOBJ replacedBitmap = SelectObject(bufferDC, bufferBitmap);
	
	if (handle == GetFocus()) {
		currentTime++;
	}
	else {
		flag = false;
	}
	
	

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
	SelectObject(displayHandle, replacedBrush);
	DeleteObject(backgroundBrush);
	SelectObject(bufferDC, replacedBitmap);
	DeleteObject(bufferBitmap);
	DeleteDC(bufferDC);

	EndPaint(handle, &structToPaint);
}

void CEllipseWindow::OnTimer()
{
	RECT clientRect;
	GetClientRect(handle, &clientRect);// Получаем прямоугольник клиентской области
	if (flag) {
		InvalidateRect(handle, &clientRect, TRUE);	// Прямоугольник для перерисовки окна
	}
}

void CEllipseWindow::OnLButtonDown()
{
	SetFocus(handle);
	flag = true;
}

void CEllipseWindow::makeEllipse(HDC hdc, int left, int top, int right, int bottom)
{
	HBRUSH insideEllipsBrush;	// Кисть для рисования эллипса

	if (handle == GetFocus()) {
		insideEllipsBrush = CreateSolidBrush(RGB(255, 0, 255));
	}
	else {
		insideEllipsBrush = CreateSolidBrush(RGB(100, 0, 100));
	}

	SelectObject(hdc, insideEllipsBrush);	// Делаем кисть для рисования эллипса активной

	Ellipse(hdc, left, top, right, bottom);
	
	DeleteObject(insideEllipsBrush);	// Заканчиваем использование кисти
}

LRESULT CEllipseWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	CEllipseWindow* window = reinterpret_cast<CEllipseWindow*>(GetWindowLongPtr(handle, GWLP_HINSTANCE));

	switch (message) 
	{
		case WM_CREATE:
		{
			window->OnCreate();
			return DefWindowProc(handle, message, wParam, lParam);
		}
		case WM_NCCREATE:
		{
			window = reinterpret_cast<CEllipseWindow*>((reinterpret_cast<CREATESTRUCT*>(lParam))->lpCreateParams);
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
		case WM_LBUTTONDOWN:
		{
			window->OnLButtonDown();
			return 0;
		}
		default:
			return DefWindowProc(handle, message, wParam, lParam);
	}
}