#include <windows.h>
#include <time.h>
#include <math.h>
#include <vector>
#include "DrawManager.h"
#include "FigureManager.h"
#include "Figure.h"

#define szWindowClass	TEXT("First")
#define szTitle			TEXT("First App")

static int mouseX = 0;
static int mouseY = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void    Rendering(HWND hWnd);

int APIENTRY WinMain( HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPSTR lpszCmdParam,
					  int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize		= sizeof(WNDCLASSEX);
	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon	= LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hIconSm= LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;

	if( !RegisterClassEx(&wcex) )
		return 0;

	HWND hWnd = CreateWindowEx( WS_EX_APPWINDOW
		, szWindowClass
		, szTitle
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, 800
		, 600
        , NULL
        , NULL
        , hInstance
        , NULL);

    if (!hWnd)
        return 0;

	ShowWindow(hWnd, nCmdShow);

	MSG	msg;
    while (true)
	{
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Rendering(hWnd);
        }
	}

	return (int)msg.wParam;
}

// 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	hdc;
	PAINTSTRUCT	ps;

	switch(message)
	{
	case WM_CREATE:

		break;
	case WM_PAINT:
		{
			hdc = BeginPaint( hWnd, &ps );
            DrawManager::getInstance()->InitScreen(hWnd);
            DrawManager::getInstance()->StartRecord();
            BitBlt(DrawManager::getInstance()->GetMemoryDC(), 0, 0, 960, 640, hdc, 0, 0, SRCCOPY);
            DrawManager::getInstance()->CloseRecord();
            DrawManager::getInstance()->DrawScreen(hWnd);
			EndPaint( hWnd, &ps );
		}
		break;
        
    case WM_MOUSEMOVE:
    {
        mouseX = LOWORD(lParam);
        mouseY = HIWORD(lParam);
    }
        break;
        
    case WM_LBUTTONDOWN:
        FigureManager::getInstance()->MakeFigure({ mouseX, mouseY }, 50, Figure::BOX);
        break;
    case WM_RBUTTONDOWN:
        FigureManager::getInstance()->MakeFigure({ mouseX, mouseY }, 50, Figure::CIRCLE);
        break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Rendering(HWND hWnd)
{
    DrawManager::getInstance()->InitScreen(hWnd);
    HDC	hdc = DrawManager::getInstance()->GetMemoryDC();

    float x1 = 200;
    float x2 = 700;
    float y1 = 500;
    float y2 = 100;

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    float a = y2 - y1;
    float b = -(x2 - x1);
    float c = -a*x1 - b*y1;
    float d = abs(a*mouseX + b*mouseY + c) / sqrt(a*a + b*b);
    
    WCHAR buf1[256] = { 0, };
    WCHAR buf2[256] = { 0, };
    WCHAR buf3[256] = { 0, };
    wsprintf(buf1, L" x = %d", mouseX);
    wsprintf(buf2, L" y = %d", mouseY);
    wsprintf(buf3, L" d = %d", (int)d);

    TextOut(hdc, 0, 1, buf1, wcslen(buf1));
    TextOut(hdc, 0, 17, buf2, wcslen(buf2));
    TextOut(hdc, 0, 33, buf3, wcslen(buf3));

    FigureManager::getInstance()->UpdateMouseFigure({ mouseX, mouseY });
    FigureManager::getInstance()->CollisionCheck();
    FigureManager::getInstance()->DrawFigure();

    DrawManager::getInstance()->DrawScreen(hWnd);
}
