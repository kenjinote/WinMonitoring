#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <wchar.h>

TCHAR szClassName[] = TEXT("Window");

struct DATA {
	HWND hWnd;
	BOOL bHit;
	LPWSTR lpszText;
};

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam) {
	DATA* pdata = (DATA*)lParam;
	if (pdata->hWnd == hWnd || IsWindowVisible(hWnd) == FALSE) return TRUE;
	DWORD dwLength = GetWindowTextLength(hWnd);
	if (dwLength != 0) {
		LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, sizeof(WCHAR) * (dwLength + 1));
		GetWindowText(hWnd, lpszText, dwLength + 1);
		wchar_t* p = wcsstr(lpszText, pdata->lpszText);
		if (p != 0) {
			pdata->bHit = TRUE;
		}
		GlobalFree(lpszText);
		if (pdata->bHit) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	DATA* pdata = (DATA*)lParam;
	if (pdata->hWnd == hWnd || IsWindowVisible(hWnd) == FALSE) return TRUE;
	DWORD dwLength = GetWindowTextLength(hWnd);
	if (dwLength != 0) {
		LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, sizeof(WCHAR) * (dwLength + 1));
		GetWindowText(hWnd, lpszText, dwLength + 1);
		wchar_t* p = wcsstr(lpszText, pdata->lpszText);
		if (p != 0) {
			pdata->bHit = TRUE;
		}
		GlobalFree(lpszText);
		if (pdata->bHit) {
			return FALSE;
		}
	}
	EnumChildWindows(hWnd, EnumChildProc, (LPARAM)pdata);
	if (pdata->bHit) {
		return FALSE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hEdit1;
	static HWND hEdit2;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("取得"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hWnd, WM_DPICHANGED, 0, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, 256, 32, TRUE);
		MoveWindow(hButton, 276, 10, 256, 32, TRUE);
		MoveWindow(hEdit2, 10, 50, LOWORD(lParam) - 20, HIWORD(lParam) - 60, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EnableWindow(hButton, FALSE);
			DWORD dwLength = GetWindowTextLength(hEdit1);
			if (dwLength != 0) {
				LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, sizeof(WCHAR) * (dwLength + 1));
				if (lpszText) {
					GetWindowText(hEdit1, lpszText, dwLength + 1);
					DATA data = {};
					data.hWnd = hWnd;
					data.lpszText = lpszText;
					data.bHit = FALSE;
					EnumWindows(EnumWindowsProc, (LPARAM)&data);
					if (data.bHit) {
						SetWindowText(hEdit2, L"Hit");
					}
					else {
						SetWindowText(hEdit2, L"Not Found");
					}
					GlobalFree(lpszText);
				}
			}
			EnableWindow(hButton, TRUE);
		}
		break;
	case WM_NCCREATE:
		{
			const HMODULE hModUser32 = GetModuleHandle(TEXT("user32.dll"));
			if (hModUser32)
			{
				typedef BOOL(WINAPI*fnTypeEnableNCScaling)(HWND);
				const fnTypeEnableNCScaling fnEnableNCScaling = (fnTypeEnableNCScaling)GetProcAddress(hModUser32, "EnableNonClientDpiScaling");
				if (fnEnableNCScaling)
				{
					fnEnableNCScaling(hWnd);
				}
			}
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPWSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
