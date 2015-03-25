#pragma once
#include <windows.h>
#include <vector>
#include "Figure.h"

class FigureManager
{
public:
    static FigureManager* getInstance()
    {
        static FigureManager figureManager;
        return &figureManager;
    }

    Figure* MakeFigure(POINT pos, int size, Figure::Type type);
    Figure* MakeFigure(POINT pos, int size, int vertex, Figure::Type type);
    void    DrawFigure();
    void    UpdateMouseFigure(POINT pos);
    void    CollisionCheck();
    void    Rotation(Figure* figure);
    void    DrawPolygon(Figure* figure);
    void    MoveFigure(Figure* figure);

private:
    bool    IsRectCollision(Figure* figureA, Figure* figureB);
    bool    IsCircleCollision(Figure* figureA, Figure* figureB);
    bool    IsRectCircleCollision(Figure* rect, Figure* circle);

private:
    FigureManager();
    ~FigureManager();
    
    std::vector<Figure*> m_FigureList;
    Figure* m_MouseFigure = nullptr;
};

