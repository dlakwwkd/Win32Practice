#pragma once
#include <windows.h>

class Figure
{
public:
    enum Type
    {
        BOX,
        CIRCLE,
    };
    Figure();
    ~Figure();

private:
    Type    m_Type;
    RECT    m_Rect;
    POINT   m_Center;
    float   m_Radius;
    bool    m_IsCollision;

    friend class FigureManager;
};

