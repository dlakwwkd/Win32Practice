#include "DrawManager.h"
#include "resource.h"

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

    HDC     imgDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = LoadBitmap((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
    SelectObject(imgDC, hBitmap);

    BitBlt(m_MemoryDC, 0, 0, BITMAP_SIZE_X, BITMAP_SIZE_Y, imgDC, 0, 0, SRCCOPY);

    DeleteDC(imgDC);
    DeleteObject(hBitmap);

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

void DrawManager::LoadBMPFile(LPCWSTR fileName)
{
    HANDLE hFile;
    DWORD FileSize, dwRead;
    BITMAPFILEHEADER fh;
    BITMAPINFO *ih;
    PVOID pRaster;

    hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }
    ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwRead, NULL);

    FileSize = fh.bfOffBits - sizeof(BITMAPFILEHEADER);
    ih = (BITMAPINFO *)malloc(FileSize);
    ReadFile(hFile, ih, FileSize, &dwRead, NULL);
    
    m_MemoryBitmap = CreateDIBSection(NULL, ih, DIB_RGB_COLORS, &pRaster, NULL, 0);
    ReadFile(hFile, pRaster, fh.bfSize - fh.bfOffBits, &dwRead, NULL);

    free(ih);
    CloseHandle(hFile);
    return;
}

void DrawManager::SaveBMPFile(LPCWSTR fileName)
{
    HANDLE hFile;
    DWORD FileSize, dwWritten;
    HDC hdc;
    BITMAP bit;
    BITMAPINFO *pih;

    int PalSize;
    BITMAPINFOHEADER bih;
    BITMAPFILEHEADER bfh;

    hdc = GetDC(NULL);
    GetObject(m_MemoryBitmap, sizeof(BITMAP), &bit);
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = bit.bmWidth;
    bih.biHeight = bit.bmHeight;
    bih.biPlanes = 1;
    bih.biBitCount = bit.bmPlanes*bit.bmBitsPixel;

    if (bih.biBitCount > 8)
        bih.biBitCount = 24;

    bih.biCompression = BI_RGB;
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    PalSize = (bih.biBitCount == 24 ? 0 : 1 << bih.biBitCount)*sizeof(RGBQUAD);
    pih = (BITMAPINFO *)malloc(bih.biSize + PalSize);
    pih->bmiHeader = bih;

    GetDIBits(hdc, m_MemoryBitmap, 0, bit.bmHeight, NULL, pih, DIB_RGB_COLORS);
    bih = pih->bmiHeader;
    if (bih.biSizeImage == 0)
    {
        bih.biSizeImage = ((((bih.biWidth*bih.biBitCount) + 31) & ~31) >> 3) * bih.biHeight;
    }
    FileSize = bih.biSize + PalSize + bih.biSizeImage;
    pih = (BITMAPINFO *)realloc(pih, FileSize);

    GetDIBits(hdc, m_MemoryBitmap, 0, bit.bmHeight, (PBYTE)pih + bih.biSize + PalSize, pih, DIB_RGB_COLORS);
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+PalSize;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfSize = FileSize + sizeof(BITMAPFILEHEADER);
    bfh.bfType = 0x4d42;

    hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        free(pih);
        return;
    }
    WriteFile(hFile, &bfh, sizeof(bfh), &dwWritten, NULL);
    WriteFile(hFile, pih, FileSize, &dwWritten, NULL);
    CloseHandle(hFile);
    ReleaseDC(NULL, hdc);
    free(pih);
    return;
}
