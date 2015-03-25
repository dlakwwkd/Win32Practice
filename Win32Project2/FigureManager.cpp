#include "FigureManager.h"
#include "DrawManager.h"
#include "KeyManager.h"

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
    figure->m_Rotation = 0;
    figure->m_IsCollision = false;
    figure->m_Rect.left = pos.x - size;
    figure->m_Rect.top = pos.y - size;
    figure->m_Rect.right = pos.x + size;
    figure->m_Rect.bottom = pos.y + size;
    m_FigureList.push_back(figure);
    return figure;
}

Figure* FigureManager::MakeFigure(POINT pos, int size, int vertex, Figure::Type m_Type)
{
    if (vertex > Figure::MAX_VERTEX)
        return nullptr;

    Figure* figure = new Figure();
    figure->m_Type = m_Type;
    figure->m_Center = pos;
    figure->m_Rotation = 0;
    figure->m_IsCollision = false;
    figure->m_VertexNum = vertex;

    POINT point = { pos.x, pos.y - size };
    for (int i = 0; i < vertex; ++i)
    {
        figure->m_VertexList[i] = point;
        if (i < vertex / 4)
        {
            point.x += size;
            point.y += size;
        }
        else if ( i < vertex / 2)
        {
            point.x -= size;
            point.y += size;
        }
        else if (i < vertex / 2 + vertex / 4)
        {
            point.x -= size;
            point.y -= size;
        }
        else
        {
            point.x += size;
            point.y -= size;
        }
    }
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
            MoveFigure(figure);
            Ellipse(hdc, figure->m_Rect.left, figure->m_Rect.top, figure->m_Rect.right, figure->m_Rect.bottom);
            break;
        case Figure::POLYGON:
            DrawPolygon(figure);
            Rotation(figure);
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
        else if (figure->m_Type == Figure::BOX && m_MouseFigure->m_Type == Figure::CIRCLE)
        {
            if (IsRectCircleCollision(figure, m_MouseFigure))
                continue;
        }
        else if (figure->m_Type == Figure::CIRCLE && m_MouseFigure->m_Type == Figure::BOX)
        {
            if (IsRectCircleCollision(m_MouseFigure, figure))
                continue;
        }
        else if (figure->m_Type == Figure::POLYGON && m_MouseFigure->m_Type == Figure::POLYGON)
        {

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

void FigureManager::Rotation(Figure* figure)
{
    figure->m_Rotation += 0.001;
}

void FigureManager::DrawPolygon(Figure* figure)
{
    HDC hdc = DrawManager::getInstance()->GetMemoryDC();
    POINT temp[Figure::MAX_VERTEX] = { 0, };
    LONG x = 0;
    LONG y = 0;
    int i = 0;
    for (auto& vert : figure->m_VertexList)
    {
        x = vert.x - figure->m_Center.x;
        y = vert.y - figure->m_Center.y;

        auto tempX = x;
        x = x * cos(figure->m_Rotation) - y * sin(figure->m_Rotation);
        y = tempX * sin(figure->m_Rotation) + y * cos(figure->m_Rotation);

        x += figure->m_Center.x;
        y += figure->m_Center.y;

        temp[i++] = { x, y };
    }
    Polygon(hdc, temp, figure->m_VertexNum);
}

void FigureManager::MoveFigure(Figure* figure)
{
    if (KeyManager::getInstance()->KeyCheck(VK_UP, HOLDKEY))
    {
        figure->m_Center.y -= 1;
        figure->m_Rect.top -= 1;
        figure->m_Rect.bottom -= 1;
    }
    if (KeyManager::getInstance()->KeyCheck(VK_DOWN, HOLDKEY))
    {
        figure->m_Center.y += 1;
        figure->m_Rect.top += 1;
        figure->m_Rect.bottom += 1;
    }
    if (KeyManager::getInstance()->KeyCheck(VK_LEFT, HOLDKEY))
    {
        figure->m_Center.x -= 1;
        figure->m_Rect.left -= 1;
        figure->m_Rect.right -= 1;
    }
    if (KeyManager::getInstance()->KeyCheck(VK_RIGHT, HOLDKEY))
    {
        figure->m_Center.x += 1;
        figure->m_Rect.left += 1;
        figure->m_Rect.right += 1;
    }
}
