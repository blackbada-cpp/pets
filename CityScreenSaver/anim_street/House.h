#ifndef House_h__
#define House_h__

#pragma once
#include "World.h"
#include <vector>


class House : public WorldObject
{
public:
   COLORREF m_frontCol;
   COLORREF m_sideCol;
   COLORREF m_windowCol;
   COLORREF m_roofCol;
   int m_windowHeight;
   int m_windowWidth;

   static double MAX_HOUSE_HEIGHT;
   static double MAX_HOUSE_WIDTH;
   static double WINDOW_HEIGHT;
   static double WINDOW_WIDTH;

public:
   House();
   House(House & other);
   House(POINT3D &pos, SIZE3D &size);
   House(int x, int y, int z, int w, int h, int d);
   ~House();
   virtual WorldObject * Clone();
   
   void GenerateHouse(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth, int maxFloorNumber, std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors);
   void SetColor(COLORREF frontCol, COLORREF sideCol);
   virtual void DoDraw(CDC3D & dc);

   static LONG GetMaxHouseWidth(RECT* prc);
   static LONG GetMaxHouseHeight(RECT* prc);
   static LONG GetWindowWidth(RECT* prc);
   static LONG GetWindowHeight(RECT* prc);
   void GenerateColor(std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors);

   void DrawRoof(CDC3D & dc, COLORREF frontCol, int x, int z, int width, int depth);
   void DrawFrontWallRect(CDC3D & dc, COLORREF frontCol, int x, int y, int width, int height);
   void DrawLeftWallRect(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height);
   void DrawRightWallRect(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height);
};

#endif // House_h__
