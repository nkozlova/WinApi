#include "CDialogWindow.h"

#include "COverlappedWindow.h"
#include <Commctrl.h>


CDialogWindow::CDialogWindow()
{
	handle = 0;
	isPreview = false;

	ZeroMemory(&setSettings, sizeof(CDisplaySettings));
	ZeroMemory(&newSettings, sizeof(CDisplaySettings));

	overlappedWindowHandle = NULL;
	editControlHandle = NULL;
}

CDialogWindow::~CDialogWindow() {}

bool CDialogWindow::Create(HWND hWndParent)
{
	overlappedWindowHandle = hWndParent;
	COverlappedWindow* ovlWindow = (COverlappedWindow*)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE);
	editControlHandle = ovlWindow->GetEditControlHandle();

	CreateDialogParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), hWndParent, dialogProc, reinterpret_cast<LPARAM>(this));
	ShowWindow(handle, SW_SHOW);

	return handle != NULL;
}

void CDialogWindow::OnInit(HWND _handle)
{
	handle = _handle;
	isPreview = false;

	COverlappedWindow* ovlWindow = (COverlappedWindow*)GetWindowLongPtr(overlappedWindowHandle, GWLP_HINSTANCE);

	// Зададим границы для изменения размера шрифта
	SendMessage(GetDlgItem(handle, IDC_FONT_SIZE), TBM_SETRANGE, TRUE, MAKELONG(8, 72));
	// Зададим границы для изменения прозрачности
	SendMessage(GetDlgItem(handle, IDC_TRANSPARENCY), TBM_SETRANGE, TRUE, MAKELONG(0, 255));

	// Получение у edit-control window и overlapped window текущих свойств
	LOGFONT logFont;
	HFONT gotFont = reinterpret_cast<HFONT>(SendMessage(editControlHandle, WM_GETFONT, 0, 0));
	GetObject(gotFont, sizeof(logFont), &logFont);

	newSettings.font = logFont;
	newSettings.transparency = ovlWindow->GetTransparency();
	newSettings.fontColor = ovlWindow->GetFontColor();
	newSettings.backgroundColor = ovlWindow->GetBackgroundColor();

	setSettings = newSettings;

	SendMessage(GetDlgItem(handle, IDC_FONT_SIZE), TBM_SETPOS, true, (LPARAM)newSettings.font.lfHeight);
	SendMessage(GetDlgItem(handle, IDC_TRANSPARENCY), TBM_SETPOS, true, (LPARAM)newSettings.transparency);

	newFont = gotFont;
	DeleteObject(gotFont);
}

INT_PTR CDialogWindow::OnCommand(WPARAM wParam)
{
	switch LOWORD(wParam) {
		case IDC_BACKGROUND_COLOR:
		{
			setNewColorTo(newSettings.backgroundColor);
			if (isPreview) {
				changeSettingsTo(newSettings);
			}
			return TRUE;
		}
		case IDC_FONT_COLOR:
		{
			setNewColorTo(newSettings.fontColor);
			if (isPreview) {
				changeSettingsTo(newSettings);
			}
			return TRUE;
		}
		case IDC_PREVIEW:
		{
			isPreview = (IsDlgButtonChecked(handle, IDC_PREVIEW) == BST_CHECKED);
			if (isPreview) {
				changeSettingsTo(newSettings);
			}
			else {
				changeSettingsTo(setSettings);
			}
			return TRUE;
		}
		case ID_OK:
		{
			changeSettingsTo(newSettings);
			EndDialog(handle, wParam);
			return TRUE;
		}
		case ID_CANCEL:
		{
			changeSettingsTo(setSettings);
			EndDialog(handle, wParam);
			return TRUE;
		}
	}

	return FALSE;
}

void CDialogWindow::OnClose(WPARAM wParam) {
	changeSettingsTo(setSettings);
	EndDialog(handle, wParam);
}

void CDialogWindow::OnScroll(WPARAM wParam, LPARAM lParam)
{
	HWND cmdHandle = reinterpret_cast<HWND>(lParam);

	if (cmdHandle == GetDlgItem(handle, IDC_FONT_SIZE)) {
		newSettings.font.lfHeight = SendMessage(GetDlgItem(handle, IDC_FONT_SIZE), TBM_GETPOS, 0, 0);
	}
	else if (cmdHandle == GetDlgItem(handle, IDC_TRANSPARENCY)) {
		newSettings.transparency = (BYTE)SendMessage(GetDlgItem(handle, IDC_TRANSPARENCY), TBM_GETPOS, 0, 0);
	}
	if (isPreview) {
		changeSettingsTo(newSettings);
	}
}

void CDialogWindow::changeSettingsTo(const CDisplaySettings &settings)
{
	COverlappedWindow* ovlWindow = (COverlappedWindow*)GetWindowLongPtr(overlappedWindowHandle, GWLP_HINSTANCE);

	DeleteObject(newFont);
	newFont = CreateFontIndirect(&settings.font);
	SendMessage(editControlHandle, WM_SETFONT, reinterpret_cast<WPARAM>(newFont), TRUE);
	SetLayeredWindowAttributes(overlappedWindowHandle, 0, settings.transparency, LWA_ALPHA);

	ovlWindow->SetTransparency(settings.transparency);
	ovlWindow->SetBackgroundColor(settings.backgroundColor);
	ovlWindow->SetFontColor(settings.fontColor);
}

void CDialogWindow::setNewColorTo(DWORD & whatToChange)
{
	COLORREF mainColors[16];
	ZeroMemory(mainColors, 16 * sizeof(COLORREF));

	CHOOSECOLOR cc = {};
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = overlappedWindowHandle;
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	cc.lpCustColors = static_cast<LPDWORD>(mainColors);
	cc.rgbResult = whatToChange;

	if (ChooseColor(&cc) == TRUE) {
		whatToChange = cc.rgbResult;
	}
}

INT_PTR CDialogWindow::dialogProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	CDialogWindow* dialog = reinterpret_cast<CDialogWindow*>(GetWindowLongPtr(handle, GWLP_HINSTANCE));

	switch (message) {
	case WM_INITDIALOG:
	{
		CDialogWindow* dialog = reinterpret_cast<CDialogWindow*>(lParam);
		SetWindowLongPtr(handle, GWLP_HINSTANCE, (LONG)dialog);
		dialog->OnInit(handle);
		return TRUE;
	}
	case WM_COMMAND:
	{
		return dialog->OnCommand(wParam);
	}
	case WM_HSCROLL:
	{
		dialog->OnScroll(wParam, lParam);
		return FALSE;
	}
	case WM_CLOSE:
		dialog->OnClose(wParam);
		return FALSE;
	default:
		return FALSE;
	}
}
