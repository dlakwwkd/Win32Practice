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


void DrawManager::MakeRandomPolygon(int x, int y, int maxSize)
{
    COLORREF color = RGB(rand() % 255, rand() % 255, rand() % 255);
    HBRUSH myBrush;

    if (rand() % 2 == 0)
        myBrush = CreateSolidBrush(color);
    else
        myBrush = CreateHatchBrush(rand() % 6, color);

    SelectObject(m_MemoryDC, myBrush);

    switch (rand() % 5)
    {
    case 0:
        Rectangle(m_MemoryDC, x - rand() % maxSize / 2, y - rand() % maxSize / 2, x + rand() % maxSize / 2, y + rand() % maxSize / 2);
        break;
    case 1:
        Ellipse(m_MemoryDC, x - rand() % maxSize / 2, y - rand() % maxSize / 2, x + rand() % maxSize / 2, y + rand() % maxSize / 2);
        break;
    case 2:
        MakeStar(x, y, maxSize / 2);
        break;
    case 3:
        MakeTriangle(x, y, maxSize / 3);
        break;
    case 4:
        MakeHexagon(x, y, maxSize / 3);
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
        Polygon(m_MemoryDC, vertexArray, vertexNum);
    }
        break;
    };
    DeleteObject(myBrush);
}

void DrawManager::MakeTriangle(int x, int y, int maxSize)
{
    POINT vertexArray[3] = { NULL, };

    vertexArray[0] = { x, y - maxSize };
    vertexArray[1] = { x - maxSize, y + maxSize };
    vertexArray[2] = { x + maxSize, y + maxSize };

    Polygon(m_MemoryDC, vertexArray, 3);
}

void DrawManager::MakeHexagon(int x, int y, int maxSize)
{
    POINT vertexArray[6] = { NULL, };

    vertexArray[0] = { x, y - maxSize };
    vertexArray[1] = { x - maxSize * 5 / 6, y - maxSize / 2 };
    vertexArray[2] = { x - maxSize * 5 / 6, y + maxSize / 2 };
    vertexArray[3] = { x, y + maxSize };
    vertexArray[4] = { x + maxSize * 5 / 6, y + maxSize / 2 };
    vertexArray[5] = { x + maxSize * 5 / 6, y - maxSize / 2 };

    Polygon(m_MemoryDC, vertexArray, 6);
}

void DrawManager::MakeStar(int x, int y, int maxSize)
{
    POINT vertexArray[5] = { NULL, };

    vertexArray[0] = { x, y - maxSize };
    vertexArray[1] = { x - maxSize * 2 / 3, y + maxSize };
    vertexArray[2] = { x + maxSize, y - maxSize / 3 };
    vertexArray[3] = { x - maxSize, y - maxSize / 3 };
    vertexArray[4] = { x + maxSize * 2 / 3, y + maxSize };

    Polygon(m_MemoryDC, vertexArray, 5);
}

