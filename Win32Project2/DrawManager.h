#pragma once
#include <windows.h>

class DrawManager
{
public:
    static DrawManager* getInstance()
    {
        static DrawManager drawManager;
        return &drawManager;
    }
    HDC     GetMemoryDC(){ return m_MemoryDC; }
    void    StartRecord(){ SelectObject(m_MemoryDC, m_MemoryBitmap); }
    void    CloseRecord(){ SelectObject(m_MemoryDC, m_ScreenBitmap); }

    void    Init(HWND hWnd);

    void    InitScreen(HWND hWnd);
    void    DrawScreen(HWND hWnd);


private:
    DrawManager();
    ~DrawManager();

    HDC     m_MemoryDC = nullptr;
    HBITMAP m_MemoryBitmap = nullptr;
    HBITMAP m_ScreenBitmap = nullptr;
};

