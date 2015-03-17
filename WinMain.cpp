#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "resource.h"
#include "DrawManager.h"

#define szWindowClass	TEXT("First")
#define szTitle			TEXT("First App")

LRESULT CALLBACK    WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HCURSOR             LoadAnimatedCursor(HINSTANCE hInstance, UINT nID, LPCTSTR pszResouceType);
void                MakeRandomPolygon(HDC hdc, int x, int y, int maxSize);
void                MakeTriangle(HDC hdc, int x, int y, int maxSize);
void                MakeHexagon(HDC hdc, int x, int y, int maxSize);
void                MakeStar(HDC hdc, int x, int y, int maxSize);
void                NewFile(HWND hWnd);
void                OpenFile(HWND hWnd);
void                SaveFile(HWND hWnd);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    srand((unsigned int)time(NULL));
	WNDCLASSEX wcex;

	wcex.cbSize		    = sizeof(WNDCLASSEX);
	wcex.style		    = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon	        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadAnimatedCursor(hInstance, IDR_MYTYPE1, L"MyType");
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
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
		, NULL );

    if (!hWnd)
		return 0;

    WCHAR appTitle[256];
    LoadString(hInstance, IDS_TITLE, appTitle, 256);
    SetWindowText(hWnd, appTitle);
    ShowWindow(hWnd, nCmdShow);

	MSG	msg;
    while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    return (int)msg.wParam;
}

// 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DrawManager::getInstance()->InitScreen(hWnd);
    HDC	hdc = DrawManager::getInstance()->GetMemoryDC();
    static bool click = false;

    switch (message)
	{
	case WM_CREATE:

		break;

    case WM_COMMAND:
    {
       switch (LOWORD(wParam))
       {
       case ID_FILE_NEW:    NewFile(hWnd);                          break;
       case ID_FILE_LOAD:   OpenFile(hWnd);                         break;
       case ID_FILE_SAVE:   SaveFile(hWnd);                         break;
       case ID_FILE_EXIT:   DestroyWindow(hWnd);                    break;
       case ID_EDIT_CLEAR:  DrawManager::getInstance()->Init(hWnd); break;
       }
    }

	case WM_PAINT:
	{
        DrawManager::getInstance()->DrawScreen(hWnd);
	}
        break;
    
    case WM_MOUSEMOVE:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        WCHAR buf1[256] = { 0, };
        WCHAR buf2[256] = { 0, };
        wsprintf(buf1, L" x = %d", x);
        wsprintf(buf2, L" y = %d", y);

        DrawManager::getInstance()->StartRecord();
        TextOut(hdc, 0, 1, buf1, wcslen(buf1));
        TextOut(hdc, 0, 17, buf2, wcslen(buf2));
        if (click) MakeRandomPolygon(hdc, x, y, 100);
        DrawManager::getInstance()->CloseRecord();

        DrawManager::getInstance()->InitScreen(hWnd);
        if (click)
        {
            HBRUSH myBrush = CreateSolidBrush(RGB(150, 150, 255));
            SelectObject(hdc, myBrush);
            Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
            DeleteObject(myBrush);
        }
        else
        {
            HBRUSH myBrush = CreateSolidBrush(RGB(50, 50, 255));
            SelectObject(hdc, myBrush);
            Ellipse(hdc, x - 20, y - 20, x + 20, y + 20);
            DeleteObject(myBrush);
        }
        DrawManager::getInstance()->DrawScreen(hWnd);
    }
        break;
    
    case WM_LBUTTONDOWN:
    {
        OutputDebugString(L"왼쪽 버튼이 클릭되었습니다. \n");

        click = true;
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        DrawManager::getInstance()->StartRecord();
        MakeRandomPolygon(hdc, x, y, 150);
        DrawManager::getInstance()->CloseRecord();

        DrawManager::getInstance()->InitScreen(hWnd);
        HBRUSH myBrush = CreateSolidBrush(RGB(150, 150, 255));
        SelectObject(hdc, myBrush);
        Ellipse(hdc, x - 25, y - 25, x + 25, y + 25);
        DeleteObject(myBrush);
        DrawManager::getInstance()->DrawScreen(hWnd);
    }
        break;
        
    case WM_LBUTTONUP:
    {
        click = false;
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        HBRUSH myBrush = CreateSolidBrush(RGB(50, 50, 255));
        SelectObject(hdc, myBrush);
        Ellipse(hdc, x - 20, y - 20, x + 20, y + 20);
        DeleteObject(myBrush);
        DrawManager::getInstance()->DrawScreen(hWnd);
    }
        break;

    case WM_RBUTTONUP:
    {
        OutputDebugString(L"오른쪽 버튼이 클릭되었습니다. \n");
        int result = MessageBox(hWnd, L"종료하시겠습니까?", L"대화상자", MB_YESNO);
        if (result == IDYES)
        {
            PostQuitMessage(0);
            return 0;
        }
    }
        break;

    case WM_LBUTTONDBLCLK:
    {
        OutputDebugString(L"왼쪽 버튼이 더블클릭되었습니다. \n");
        int result = MessageBox(hWnd, L"화면을 초기화 하시겠습니까?", L"대화상자", MB_YESNO);
        if (result == IDYES)
        {
            DrawManager::getInstance()->Init(hWnd);
        }
    }
        break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HCURSOR LoadAnimatedCursor(HINSTANCE hInstance, UINT nID, LPCTSTR pszResouceType)
{
    HCURSOR hCursor = NULL;
    if (hInstance)
    {
        HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(nID), pszResouceType);
        DWORD dwResourceSize = SizeofResource(hInstance, hResource);
        if (dwResourceSize > 0)
        {
            HGLOBAL hRsrcGlobal = LoadResource(hInstance, hResource);
            if (hRsrcGlobal)
            {
                LPBYTE pResource = (LPBYTE)LockResource(hRsrcGlobal);
                if (pResource)
                {
                    hCursor = (HCURSOR)CreateIconFromResource(pResource, dwResourceSize, FALSE, 0x00030000);
                    UnlockResource(pResource);
                }
                FreeResource(hRsrcGlobal);
            }
        }
    }
    return hCursor;
}

void MakeRandomPolygon(HDC hdc, int x, int y, int maxSize)
{
    COLORREF color = RGB(rand() % 255, rand() % 255, rand() % 255);
    HBRUSH myBrush;

    if (rand() % 2 == 0)
        myBrush = CreateSolidBrush(color);
    else
        myBrush = CreateHatchBrush(rand() % 6, color);

    SelectObject(hdc, myBrush);

    switch (rand() % 5)
    {
    case 0:
        Rectangle(hdc, x - rand() % maxSize / 2, y - rand() % maxSize / 2, x + rand() % maxSize / 2, y + rand() % maxSize / 2);
        break;
    case 1:
        Ellipse(hdc, x - rand() % maxSize / 2, y - rand() % maxSize / 2, x + rand() % maxSize / 2, y + rand() % maxSize / 2);
        break;
    case 2:
        MakeStar(hdc, x, y, maxSize / 2);
        break;
    case 3:
        MakeTriangle(hdc, x, y, maxSize / 3);
        break;
    case 4:
        MakeHexagon(hdc, x, y, maxSize / 3);
        break;
    case 5:
    case 6:
    {
        POINT vertexArray[12] = { NULL, };
        int vertexNum = rand() % 12 + 1;

        for (int i = 0; i < vertexNum; ++i)
        {
            vertexArray[i] = { x + rand() % maxSize - maxSize / 2, y + rand() % maxSize - maxSize / 2 };
        }
        Polygon(hdc, vertexArray, vertexNum);
    }
        break;
    };
    DeleteObject(myBrush);
}

void MakeTriangle(HDC hdc, int x, int y, int maxSize)
{
    POINT vertexArray[3] = { NULL, };

    vertexArray[0] = { x, y - maxSize };
    vertexArray[1] = { x - maxSize, y + maxSize };
    vertexArray[2] = { x + maxSize, y + maxSize };

    Polygon(hdc, vertexArray, 3);
}

void MakeHexagon(HDC hdc, int x, int y, int maxSize)
{
    POINT vertexArray[6] = { NULL, };

    vertexArray[0] = { x, y - maxSize };
    vertexArray[1] = { x - maxSize * 5 / 6, y - maxSize / 2 };
    vertexArray[2] = { x - maxSize * 5 / 6, y + maxSize / 2 };
    vertexArray[3] = { x, y + maxSize };
    vertexArray[4] = { x + maxSize * 5 / 6, y + maxSize / 2 };
    vertexArray[5] = { x + maxSize * 5 / 6, y - maxSize / 2 };

    Polygon(hdc, vertexArray, 6);
}

void MakeStar(HDC hdc, int x, int y, int maxSize)
{
    POINT vertexArray[5] = { NULL, };

    vertexArray[0] = { x, y - maxSize };
    vertexArray[1] = { x - maxSize * 2 / 3, y + maxSize };
    vertexArray[2] = { x + maxSize, y - maxSize / 3 };
    vertexArray[3] = { x - maxSize, y - maxSize / 3 };
    vertexArray[4] = { x + maxSize * 2 / 3, y + maxSize };

    Polygon(hdc, vertexArray, 5);
}

void NewFile(HWND hWnd)
{
    int result = MessageBox(hWnd, L"저장하시겠습니까?", L"새로 만들기", MB_YESNOCANCEL);
    switch (result)
    {
    case IDYES:
        SaveFile(hWnd);
        DrawManager::getInstance()->Init(hWnd);
        break;
    case IDNO:
        DrawManager::getInstance()->Init(hWnd);
        break;
    case IDCANCEL:
        break;
    }
}

void OpenFile(HWND hWnd)
{
    WCHAR lpstrFile[MAX_PATH] = L"";
    OPENFILENAME OFN;

    ZeroMemory(&OFN, sizeof(OPENFILENAME));
    OFN.lStructSize = sizeof(OPENFILENAME);
    OFN.hwndOwner = hWnd;
    OFN.lpstrFilter = L"Every File(*.*)\0*.*\0Bitmap File(.bmp)\0*.bmp\0";
    OFN.lpstrFile = lpstrFile;
    OFN.nMaxFile = 256;
    if (GetOpenFileName(&OFN) != 0)
    {
        DrawManager::getInstance()->LoadBMPFile(lpstrFile);
    }
}

void SaveFile(HWND hWnd)
{
    WCHAR lpstrFile[MAX_PATH] = L"";
    OPENFILENAME OFN;

    ZeroMemory(&OFN, sizeof(OPENFILENAME));
    OFN.lStructSize = sizeof(OPENFILENAME);
    OFN.hwndOwner = hWnd;
    OFN.lpstrFilter = L"Every File(*.*)\0*.*\0Bitmap File(.bmp)\0*.bmp\0";
    OFN.lpstrFile = lpstrFile;
    OFN.nMaxFile = 256;
    OFN.lpstrDefExt = L"bmp";
    OFN.lpstrTitle = L"저장할 파일 이름을 입력하세요";
    if (GetSaveFileName(&OFN) != 0)
    {
        DrawManager::getInstance()->SaveBMPFile(lpstrFile);
    }
}
