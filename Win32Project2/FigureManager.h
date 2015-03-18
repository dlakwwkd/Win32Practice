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
    void    DrawFigure();
    void    UpdateMouseFigure(POINT pos);
    void    CollisionCheck();

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

