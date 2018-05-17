#include <atlstr.h> // CString
#include "COverlappedWindow.h"


#define MAX_PATH 256

COverlappedWindow::COverlappedWindow()
{
	childEditControlWindow = CEditControlWindow();
	dialogWindow = CDialogWindow();
	textWasEntered = false;
	activeBrush = NULL;
}

COverlappedWindow::~COverlappedWindow() {}

bool COverlappedWindow::RegisterClass()
{
	WNDCLASSEX windowClass;

	// Заполним структуру соответствующими данными
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = COverlappedWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.style = CS_HREDRAW;
	windowClass.lpszClassName = L"COverlappedWindow";

	return RegisterClassEx(&windowClass);
}

bool COverlappedWindow::Create()
{
	CreateWindowEx(WS_EX_LAYERED | WS_EX_OVERLAPPEDWINDOW,
		L"COverlappedWindow",
		L"Main window",
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
	childEditControlWindow.Show(cmdShow);
}

void COverlappedWindow::OnCreate()
{
	childEditControlWindow.Create(handle);

	// Создание меню
	windowMenu = CreateMenu();
	HMENU hPopMenuFile = CreatePopupMenu();

	AppendMenu(windowMenu, MF_STRING | MF_POPUP, (UINT)hPopMenuFile, L"&View");
	AppendMenu(hPopMenuFile, MF_STRING, 0, L"&Settings");

	SetMenu(handle, windowMenu);
	SetMenu(handle, hPopMenuFile);

	SetLayeredWindowAttributes(handle, 0, 255, LWA_COLORKEY);
	transparency = 255;
	fontColor = RGB(0, 0, 0);
	backgroundColor = RGB(255, 255, 255);

	SetLayeredWindowAttributes(handle, 0, transparency, LWA_ALPHA);
	
	LOGFONT logFont;
	logFont.lfHeight = 18;
	logFont.lfWidth = NULL;
	logFont.lfEscapement = NULL;
	logFont.lfOrientation = NULL;
	logFont.lfWeight = NULL;// FW_DONTCARE;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = FALSE;
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfPitchAndFamily = DEFAULT_PITCH;// | FF_SWISS;

	HFONT hFont = CreateFontIndirect(&logFont);

	SendMessage(childEditControlWindow.GetWindowHandle(), WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
}

void COverlappedWindow::OnNCCreate(HWND _handle)
{
	handle = _handle;
}

void COverlappedWindow::OnSize()
{
	RECT clientRect;
	GetClientRect(handle, &clientRect);
	SetWindowPos(childEditControlWindow.GetWindowHandle(),
		HWND_TOP, clientRect.left, clientRect.top,
		clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 0);
}

void COverlappedWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam))
	{
		case 0:
		{
			OnCommandMenu(wParam);
			break;
		}
		case EN_CHANGE:
		{
			textWasEntered = true;
		}
		default:
			break;
	}
}

void COverlappedWindow::OnCommandMenu(WPARAM wParam)
{
	dialogWindow.Create(handle);
}

bool COverlappedWindow::OnClose()
{
	if (textWasEntered) {
		int messageBoxCode = MessageBox(handle,
			CString("Сохранить введённый текст?"),
			CString("Закрыть приложение"),
			MB_YESNOCANCEL);

		switch (messageBoxCode)
		{
			case IDYES:		// Да - сохранить и закрыть, 
			{
				saveText();
				return true;
			}
			case IDNO:		// Нет - не сохранять и закрыть, 
			{
				return true;
			}
			case IDCANCEL:		// Отмена - не закрывать
			{
				return false; // не закрывать
			}
		}
	}
	return true;
}

void COverlappedWindow::OnDestroy()
{
	if (activeBrush != NULL) {
		DeleteObject(activeBrush);
	}
	DestroyMenu(windowMenu);

	HFONT currentFont = reinterpret_cast<HFONT>(SendMessage(childEditControlWindow.GetWindowHandle(), WM_GETFONT, 0, 0));
	DeleteObject(currentFont);

	PostQuitMessage(0);
}

LRESULT COverlappedWindow::OnControlColorEdit(HDC hdc)
{
	if (activeBrush != NULL) {
		DeleteObject(activeBrush);
	}

	SetTextColor(hdc, fontColor);
	SetBkColor(hdc, backgroundColor);

	activeBrush = CreateSolidBrush(backgroundColor);
	return reinterpret_cast<LRESULT>(activeBrush);
}

void COverlappedWindow::saveText()
{
	// Получим длину текста edit-контрола
	int textLength = SendMessage(childEditControlWindow.GetWindowHandle(),
		WM_GETTEXTLENGTH, NULL, NULL);

	// Получим сам текст edit-контрола
	std::shared_ptr<wchar_t[]> textFromECWin(new wchar_t[textLength]);
	LRESULT gettingTextResult = SendMessage(childEditControlWindow.GetWindowHandle(),
		WM_GETTEXT, textLength, reinterpret_cast<LPARAM>(textFromECWin.get()));

	// Сохранение текста в файл
	char savingFileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = static_cast<LPCWSTR>(L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
	ofn.lpstrFile = reinterpret_cast<LPWSTR>(savingFileName);
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = static_cast<LPCWSTR>(L"txt");

	if (GetSaveFileName(&ofn)) {
		HANDLE createdFile = CreateFile(reinterpret_cast<LPCWSTR>(savingFileName),
			GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		WriteFile(createdFile, textFromECWin.get(), textLength * sizeof(wchar_t), 0, 0);
		CloseHandle(createdFile);
	}
}

LRESULT COverlappedWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	COverlappedWindow* window = (COverlappedWindow*)GetWindowLongPtr(handle, GWLP_HINSTANCE);
	switch (message) {
	case WM_CREATE:
	{
		window->OnCreate();
		return FALSE;
	}
	case WM_NCCREATE:
	{
		COverlappedWindow* window = (COverlappedWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLongPtr(handle, GWLP_HINSTANCE, (LONG)window);
		window->OnNCCreate(handle);
		return TRUE;
	}
	case WM_SIZE:
	{
		window->OnSize();
		return FALSE;
	}
	case WM_COMMAND:
	{
		window->OnCommand(wParam, lParam);
		return DefWindowProc(handle, message, wParam, lParam);
	}
	case WM_CLOSE:
	{
		if (window->OnClose()) { // нужно закрыть окно
			return DefWindowProc(handle, message, wParam, lParam);
		}
		else {
			return FALSE;
		}
	}
	case WM_DESTROY:
	{
		window->OnDestroy();
		return FALSE;
	}
	case WM_CTLCOLOREDIT:
	{
		return window->OnControlColorEdit(reinterpret_cast<HDC>(wParam));
	}
	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
}