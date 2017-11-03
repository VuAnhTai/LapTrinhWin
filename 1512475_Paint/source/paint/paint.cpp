// paint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "paint.h"
#include "Line.h"
#include "Rectangle.h"
#include "Ellipse.h"

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
bool isDrawing = FALSE;
HWND gHPaint;
POINT pCurrent, pEnd;
HPEN gHpen;
WCHAR *openFileDirec = NULL;
int isOpenFile = 0;
int sizeOfPen = 1;				// mặc định nét vẽ ban đầu là 1 pixel
int iChoose = LINE;				// mặc định ban đầu là vẽ LINE
COLORREF iColor = RGB(0, 0, 0);	// mặc định màu bút ban đầu là đen
vector<CShape*> gShapes;
vector<CShape*> gPrototypes;
HMENU hMenu;
POINT pRMouse;
RECT rect;

//____________________ my Function ____________________
void ChangeToSpecial();

//____________________WM function______________________
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnButtonDown(HWND hwnd, int dc, int x, int y, LPARAM lParam);
void OnButtonUp(HWND hwnd, int x, int wParam, LPARAM lParam);
void OnMouseMove(HWND hwnd, int x, int y, LPARAM lParam);
void OnPaint(HWND hwnd);
void OnDestroy(HWND hwnd);

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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
	LoadString(hInstance, IDC_PAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PAINT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnButtonUp);
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



BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct){
	hMenu = GetMenu(hWnd);
	//đặt mặc định là line
	CheckMenuItem(hMenu, ID_SHAPES_LINE, MF_CHECKED);
	gPrototypes.push_back(new CLine);
	//gPrototypes.push_back(new CRectangle);
	return true;
}

void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify){
	hMenu = GetMenu(hWnd);

	switch (id)
	{
	case ID_SHAPES_LINE:
	{
		CheckMenuItem(hMenu, ID_SHAPES_LINE, MF_CHECKED);
		CheckMenuItem(hMenu, ID_SHAPES_RECTANGLE, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_SHAPES_ELLIPSE, MF_UNCHECKED);
		iChoose = LINE;
		break;
	}
	case ID_SHAPES_RECTANGLE:
	{
		CheckMenuItem(hMenu, ID_SHAPES_LINE, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_SHAPES_RECTANGLE, MF_CHECKED);
		CheckMenuItem(hMenu, ID_SHAPES_ELLIPSE, MF_UNCHECKED);
		iChoose = RECTANGLE;
		break;
	}
	case ID_SHAPES_ELLIPSE:
	{
		CheckMenuItem(hMenu, ID_SHAPES_LINE, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_SHAPES_RECTANGLE, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_SHAPES_ELLIPSE, MF_CHECKED);
		iChoose = ELLIPSE;
		break;
	}
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	}
}

void OnButtonDown(HWND hwnd, int dc, int x, int y, LPARAM lParam)
{
	if (!isDrawing)
	{
		isDrawing = TRUE;
		pCurrent.x = x;
		pCurrent.y = y;
	}
}

void OnButtonUp(HWND hWnd, int x, int y, LPARAM lParam)
{
	pEnd.x = x;
	pEnd.y = y;

	if (iChoose == LINE)
	{
		CLine* line = new CLine;
		line->SetData(pCurrent.x, pCurrent.y, pEnd.x, pEnd.y, iColor, sizeOfPen);
		gShapes.push_back(line);
	}
	else if (iChoose == RECTANGLE)
	{
		if (lParam & MK_SHIFT)		//Nếu user vẫn giữ SHIFT khi thả chuột thì vẽ H.vuông
			ChangeToSpecial();
		CRectangle* rect = new CRectangle;
		rect->SetData(pCurrent.x, pCurrent.y, pEnd.x, pEnd.y, iColor, sizeOfPen);
		gShapes.push_back(rect);
	}
	else
	{
		if (lParam & MK_SHIFT)		//Nếu user vẫn giữ SHIFT khi thả chuột thì vẽ H.tròn
			ChangeToSpecial();
		CEllipse* ellipse = new CEllipse;
		ellipse->SetData(pCurrent.x, pCurrent.y, pEnd.x, pEnd.y, iColor, sizeOfPen);
		gShapes.push_back(ellipse);
	}

	isDrawing = FALSE;
	InvalidateRect(hWnd, NULL, FALSE);
}

void OnMouseMove(HWND hWnd, int x, int y, LPARAM lParam)
{
	if (isDrawing)
	{
		pEnd.x = x;
		pEnd.y = y;
		if ((lParam & MK_SHIFT) && iChoose != 1)		//Nếu user giữ SHIFT khi kéo chuột thì vẽ H.vuông (Rectangle) / H.tròn (Ellipse) (preview)
			ChangeToSpecial();
		InvalidateRect(hWnd, NULL, FALSE);
	}
}

void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HBRUSH hbr = GetStockBrush(NULL_BRUSH);
	HPEN hpen = CreatePen(0, sizeOfPen, iColor);

	GetClientRect(hWnd, &rect);
	HDC vHdc = CreateCompatibleDC(hdc);
	HBITMAP vBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	SelectObject(vHdc, vBitmap);
	FillRect(vHdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

	SelectObject(vHdc, hbr);
	SelectObject(vHdc, hpen);

	for (int i = 0; i < gShapes.size(); i++) {
		gShapes[i]->Draw(vHdc);
	}

	if (isDrawing) {
		if (iChoose == LINE)
		{
			MoveToEx(vHdc, pCurrent.x, pCurrent.y, NULL);
			LineTo(vHdc, pEnd.x, pEnd.y);
		}
		else if (iChoose == RECTANGLE)
		{
			Rectangle(vHdc, pCurrent.x, pCurrent.y, pEnd.x, pEnd.y);
		}
		else
		{
			Ellipse(vHdc, pCurrent.x, pCurrent.y, pEnd.x, pEnd.y);
		}
	}

	BitBlt(hdc, 0, 0, rect.right, rect.bottom, vHdc, 0, 0, SRCCOPY);
	DeleteObject(hpen);
	EndPaint(hWnd, &ps);
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

void ChangeToSpecial()
{
	if (abs(pCurrent.x - pEnd.x) > abs(pCurrent.y - pEnd.y))
	{
		if (pCurrent.x > pEnd.x)
			pEnd.x = pCurrent.x - abs(pCurrent.y - pEnd.y);
		else
			pEnd.x = pCurrent.x + abs(pCurrent.y - pEnd.y);
	}
	else
	{
		if (pCurrent.y > pEnd.y)
			pEnd.y = pCurrent.y - abs(pCurrent.x - pEnd.x);
		else
			pEnd.y = pCurrent.y + abs(pCurrent.x - pEnd.x);
	}
}