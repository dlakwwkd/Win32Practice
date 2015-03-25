#pragma once
#include <windows.h>

class Figure
{
public:
    enum
    {
        MAX_VERTEX =  4
    };
    enum Type
    {
        BOX,
        CIRCLE,
        POLYGON,
    };
    Figure();
    ~Figure();

private:
    Type    m_Type;
    RECT    m_Rect;
    POINT   m_Center;
    float   m_Radius;
    double  m_Rotation;
    bool    m_IsCollision;
    int     m_VertexNum;
    POINT   m_VertexList[MAX_VERTEX];

    friend class FigureManager;
};

