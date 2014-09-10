// OSISP1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "OSISP1.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;
HDC hScreenDC = NULL, hMemoryDC = NULL;
HBITMAP hPicture = NULL;
DWORD *data = NULL; // указатель на область пам€ти, где будет лежать картинка
int width, height;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OSISP1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSISP1));

	// Main message loop:
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
	
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSISP1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OSISP1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


bool InitializeGraphics(HWND hw)
{
	// проверка окна
	if (!IsWindow(hw))
		return false;

	// если процедура вызвана дл€ –≈-инициализации, освободим существующие объекты
	if (hPicture)
		DeleteObject(hPicture);
	if (hMemoryDC)
		DeleteDC(hMemoryDC);
	if (hScreenDC)
		ReleaseDC(hw, hScreenDC);

	// найдем размеры "окна" дл€ рисовани€
	RECT rect;
	GetClientRect(hw, &rect);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// захват контекста устройства окна (клиентской области)
	hScreenDC = GetDC(hw);
	if (hScreenDC == NULL)
		return false;

	// создание совместимого контекста устройства в пам€ти - будущий буфер
	hMemoryDC = CreateCompatibleDC(hScreenDC);
	if (hMemoryDC == NULL)
	{
		ReleaseDC(hw, hScreenDC);
		return false;
	}

	// создание BITMAP дл€ рисовани€ 
	LPBITMAPINFO lpbi;
	lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)+(3 * sizeof(RGBQUAD))];
	BITMAPINFO &MSbi = *lpbi;
	ZeroMemory(&MSbi, sizeof(BITMAPINFO));

	// параметры хранени€ картинки в пам€ти
	MSbi.bmiHeader.biBitCount = 32; // 32 бит на точку - одна точка - одно 32-битное слово
	MSbi.bmiHeader.biClrImportant = 0;
	MSbi.bmiHeader.biClrUsed = 0;
	MSbi.bmiHeader.biCompression = BI_RGB; // без компрессии
	MSbi.bmiHeader.biHeight = -height;  // знак "-" нужен чтобы битмап не был "перевернут" - 
	// при "+" первыми в буфере идут последние строчки битмапа
	MSbi.bmiHeader.biPlanes = 1;// количество цветовых плоскостей, дл€ RGB всегда 1
	MSbi.bmiHeader.biSize = sizeof(BITMAPINFO);
	MSbi.bmiHeader.biSizeImage = 0;
	MSbi.bmiHeader.biWidth = width;
	MSbi.bmiHeader.biXPelsPerMeter = 0;
	MSbi.bmiHeader.biYPelsPerMeter = 0;

	// создаем устройство-независимую битмап-секцию (Device-Independent Bitmap, DIB)
	hPicture = CreateDIBSection(hMemoryDC, &MSbi, DIB_RGB_COLORS, (void**)&(data), NULL, NULL);
	if (hPicture == NULL)
	{
		DWORD k = GetLastError();
		ReleaseDC(hw, hScreenDC);
		DeleteDC(hMemoryDC);
		return false;
	}

	// выбираем созданную секцию на memoryDC - т.е. дл€ буфера
	SelectObject(hMemoryDC, hPicture);
	static HBRUSH Brush = CreateSolidBrush(RGB(255,255,255));
	FillRect(hMemoryDC, &rect, Brush);

	delete lpbi;
	lpbi = NULL;

	return true;
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd)
   {
      return FALSE;
   }
   InitializeGraphics(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	static HPEN Pen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
	SelectObject(hScreenDC, Pen);
	SelectObject(hMemoryDC, Pen);
	static bool isActionActivated = false;
	static int xPosition = 0;
	static int yPosition = 0;
	switch (message)
	{
		case WM_MOUSEMOVE:
		{
			//do nothing
			if (isActionActivated)
			{
				MoveToEx(hMemoryDC, xPosition, yPosition, NULL);
				xPosition = LOWORD(lParam);
				yPosition = HIWORD(lParam);
				LineTo(hMemoryDC, xPosition, yPosition);
				InvalidateRect(hWnd, NULL, false);
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			isActionActivated = true;
			xPosition = LOWORD(lParam);
			yPosition = HIWORD(lParam);
			break;
		}
		case WM_LBUTTONUP:
		{
			isActionActivated = false;
			break;
		}
		case WM_SIZE:   // перемещении/изменении размеров окна
			if (InitializeGraphics(hWnd)) // (пере)инициализируем графику
			{
				// и выбираем белый карандаш на оба контекста устройства
				SelectObject(hScreenDC, Pen);
				SelectObject(hMemoryDC, Pen);
			}
			if (message != WM_CREATE)   // если окно уже было создано
				InvalidateRect(hWnd, NULL, false); // перерисуем
			break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hScreenDC = BeginPaint(hWnd, &ps);
		BitBlt(hScreenDC, 0, 0, width, height, hMemoryDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
