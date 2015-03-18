#include "FigureManager.h"
#include "DrawManager.h"

#define INIT_FIGURE_NUM 100

FigureManager::FigureManager()
{
    m_FigureList.reserve(INIT_FIGURE_NUM);
}


FigureManager::~FigureManager()
{
    for (auto& figure : m_FigureList)
    {
        delete figure;
    }
}

Figure* FigureManager::MakeFigure(POINT pos, int size, Figure::Type m_Type)
{
    Figure* figure = new Figure();
    figure->m_Type = m_Type;
    figure->m_Center = pos;
    figure->m_Radius = size;
    figure->m_IsCollision = false;
    figure->m_Rect.left = pos.x - size;
    figure->m_Rect.top = pos.y - size;
    figure->m_Rect.right = pos.x + size;
    figure->m_Rect.bottom = pos.y + size;
    m_FigureList.push_back(figure);
    return figure;
}

void FigureManager::DrawFigure()
{
    HDC hdc = DrawManager::getInstance()->GetMemoryDC();

    for (auto& figure : m_FigureList)
    {
        HBRUSH myBrush;
        HBRUSH oldBrush;
        if (figure->m_IsCollision)
        {
            myBrush = CreateSolidBrush(RGB(255, 50, 50));
            oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
        }
        switch (figure->m_Type)
        {
        case Figure::BOX:
            Rectangle(hdc, figure->m_Rect.left, figure->m_Rect.top, figure->m_Rect.right, figure->m_Rect.bottom);
            break;
        case Figure::CIRCLE:
            Ellipse(hdc, figure->m_Rect.left, figure->m_Rect.top, figure->m_Rect.right, figure->m_Rect.bottom);
            break;
        }
        if (figure->m_IsCollision)
        {
            SelectObject(hdc, oldBrush);
            DeleteObject(myBrush);
        }
    }
}

void FigureManager::UpdateMouseFigure(POINT pos)
{
    if (!m_MouseFigure)
    {
        m_MouseFigure = MakeFigure(pos, 50, Figure::CIRCLE);
    }
    m_MouseFigure->m_Center = pos;
    m_MouseFigure->m_Rect.left = pos.x - 50;
    m_MouseFigure->m_Rect.top = pos.y - 50;
    m_MouseFigure->m_Rect.right = pos.x + 50;
    m_MouseFigure->m_Rect.bottom = pos.y + 50;
}

void FigureManager::CollisionCheck()
{
    for (auto& figure : m_FigureList)
    {
        if (figure == m_MouseFigure)
            continue;

        figure->m_IsCollision = true;
        m_MouseFigure->m_IsCollision = true;

        if (figure->m_Type == Figure::BOX && m_MouseFigure->m_Type == Figure::BOX)
        {
            if (IsRectCollision(figure, m_MouseFigure))
                continue;
        }
        else if (figure->m_Type == Figure::CIRCLE && m_MouseFigure->m_Type == Figure::CIRCLE)
        {
            if (IsCircleCollision(figure, m_MouseFigure))
                continue;
        }
        else
        {
            if (figure->m_Type == Figure::BOX)
            {
                if (IsRectCircleCollision(figure, m_MouseFigure))
                    continue;
            }
            else
            {
                if (IsRectCircleCollision(m_MouseFigure, figure))
                    continue;
            }
        }
        figure->m_IsCollision = false;
        m_MouseFigure->m_IsCollision = false;
    }
}

bool FigureManager::IsRectCollision(Figure* figureA, Figure* figureB)
{
    RECT temp;
    if (IntersectRect(&temp, &figureA->m_Rect, &figureB->m_Rect))
        return true;
    return false;
}

bool FigureManager::IsCircleCollision(Figure* figureA, Figure* figureB)
{
    float r = figureA->m_Radius + figureB->m_Radius;
    float x = figureA->m_Center.x - figureB->m_Center.x;
    float y = figureA->m_Center.y - figureB->m_Center.y;
    float distance = sqrt(pow(x, 2) + pow(y, 2));
    if (r > distance)
        return true;
    return false;
}

bool FigureManager::IsRectCircleCollision(Figure* rect, Figure* circle)
{
    {
        POINT vert1 = { circle->m_Rect.left, circle->m_Center.y };
        POINT vert2 = { circle->m_Center.x, circle->m_Rect.top };
        POINT vert3 = { circle->m_Rect.right, circle->m_Center.y };
        POINT vert4 = { circle->m_Center.x, circle->m_Rect.bottom };
        POINT vertList[] = { vert1, vert2, vert3, vert4 };

        for (auto& vert : vertList)
        {
            if (PtInRect(&rect->m_Rect, vert))
                return true;
        }
    }

    POINT vert1 = { rect->m_Rect.left, rect->m_Rect.top };
    POINT vert2 = { rect->m_Rect.left, rect->m_Rect.bottom };
    POINT vert3 = { rect->m_Rect.right, rect->m_Rect.top };
    POINT vert4 = { rect->m_Rect.right, rect->m_Rect.bottom };
    POINT vertList[] = { vert1, vert2, vert3, vert4 };

    for (auto& vert : vertList)
    {
        float x = vert.x - circle->m_Center.x;
        float y = vert.y - circle->m_Center.y;
        float distance = sqrt(pow(x, 2) + pow(y, 2));
        if (circle->m_Radius > distance)
            return true;
    }
    return false;
}
