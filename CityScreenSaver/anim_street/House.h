#ifndef House_h__
#define House_h__

#pragma once
#include "World.h"


class House : public WorldObject
{
public:
   COLORREF m_frontCol;
   COLORREF m_sideCol;
   COLORREF m_windowCol;
   COLORREF m_roofCol;
   int m_windowHeight;
   int m_windowWidth;

public:
   House();
   House(POINT3D &pos, SIZE3D &size);
   House(int x, int y, int z, int w, int h, int d);
   ~House();
   
   void SetColor(COLORREF frontCol, COLORREF sideCol);
   virtual void DoDraw(CDC3D & dc);

   static LONG GetHouseWidth(RECT* prc);
   static LONG GetHouseHeight(RECT* prc);
   static LONG GetWindowWidth(RECT* prc);
   static LONG GetWindowHeight(RECT* prc);
   static void GenerateColor(COLORREF & frontCol, COLORREF & sideCol);
   
   void DrawRoof(CDC3D & dc, COLORREF frontCol, int x, int z, int width, int depth);
   void DrawFrontWallRect(CDC3D & dc, COLORREF frontCol, int x, int y, int width, int height);
   void DrawLeftWallRect(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height);
   void DrawRightWallRect(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height);
};

#endif // House_h__
