#include "DrawManager.h"

#define BITMAP_SIZE_X 960
#define BITMAP_SIZE_Y 640

DrawManager::DrawManager()
{
}


DrawManager::~DrawManager()
{
    DeleteDC(m_MemoryDC);
    DeleteObject(m_MemoryBitmap);
    DeleteObject(m_ScreenBitmap);
}

void DrawManager::Init(HWND hWnd)
{
    if (m_MemoryDC || m_MemoryBitmap || m_ScreenBitmap)
    {
        DeleteDC(m_MemoryDC);
        DeleteObject(m_MemoryBitmap);
        DeleteObject(m_ScreenBitmap);
    }
    HDC	hdc;
    PAINTSTRUCT	ps;
    hdc = BeginPaint(hWnd, &ps);

    m_MemoryDC = CreateCompatibleDC(hdc);
    m_MemoryBitmap = CreateCompatibleBitmap(hdc, BITMAP_SIZE_X, BITMAP_SIZE_Y);
    m_ScreenBitmap = CreateCompatibleBitmap(hdc, BITMAP_SIZE_X, BITMAP_SIZE_Y);
    SelectObject(m_MemoryDC, m_MemoryBitmap);

    EndPaint(hWnd, &ps);
}

void DrawManager::InitScreen(HWND hWnd)
{
    if (!m_MemoryDC)
    {
        Init(hWnd);
    }
    HDC imgDC = CreateCompatibleDC(m_MemoryDC);
    SelectObject(imgDC, m_MemoryBitmap);
    SelectObject(m_MemoryDC, m_ScreenBitmap);

    BitBlt(m_MemoryDC, 0, 0, BITMAP_SIZE_X, BITMAP_SIZE_Y, imgDC, 0, 0, SRCCOPY);

    DeleteDC(imgDC);
}

void DrawManager::DrawScreen(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);
    SelectObject(m_MemoryDC, m_ScreenBitmap);
    BitBlt(hdc, 0, 0, BITMAP_SIZE_X, BITMAP_SIZE_Y, m_MemoryDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hdc);
}

