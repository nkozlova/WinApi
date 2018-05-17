#include "CEditControlWindow.h"


CEditControlWindow::CEditControlWindow() {}

CEditControlWindow::~CEditControlWindow() {}

bool CEditControlWindow::Create(HWND hWndParent)
{
	handle = CreateWindowEx(NULL,
		L"EDIT",
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWndParent,
		NULL,
		GetModuleHandle(NULL),
		NULL);

	SetFocus(handle);

	return handle != NULL;
}

void CEditControlWindow::Show(int cmdShow)
{
	ShowWindow(handle, cmdShow);
}