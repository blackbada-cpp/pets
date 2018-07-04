#ifndef House_h__
#define House_h__

#pragma once
#include "World.h"
#include <vector>


enum HouseStyle
{
   style_CountryHouse,
   style_SkyScraper,
   style_OldTown
};


class House : public WorldObject
{
public:
   COLORREF m_frontCol;
   COLORREF m_sideCol;
   COLORREF m_windowCol;
   COLORREF m_roofCol;
   int m_windowHeight;
   int m_windowWidth;

   bool m_DoDraw_LeftWallVisible;
   bool m_DoDraw_RightWallVisible;

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
   //virtual WorldObject * Clone();
   
   void Generate(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth, int maxFloorNumber, std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors);
   void SetColor(COLORREF frontCol, COLORREF sideCol);
   virtual void DoDraw(CDC3D & dc);

   static LONG GetMaxHouseWidth(RECT* prc);
   static LONG GetMaxHouseHeight(RECT* prc);
   static LONG GetWindowWidth(RECT* prc);
   static LONG GetWindowHeight(RECT* prc);
   static LONG GetPlantWidth(RECT* prc);
   static LONG GetPlantHeight(RECT* prc);
   void GenerateColor(std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors);

   void DrawRoof(CDC3D & dc, COLORREF frontCol, int x, int z, int width, int depth);
   
   //Draw filled rectangles on walls
   void DrawFrontWallPolygon(CDC3D & dc, COLORREF frontCol, int x, int y, int width, int height);
   bool DrawLeftWallPolygon(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height);
   bool DrawRightWallPolygon(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height);

   //Draw polylines on walls
   void DrawFrontWallLine(CDC3D & dc, COLORREF frontCol, int x1, int y1, int x2, int y2);
   void DrawLeftWallLine(CDC3D & dc, COLORREF frontCol, int z1, int y1, int z2, int y2);
   void DrawRightWallLine(CDC3D & dc, COLORREF frontCol, int z1, int y1, int z2, int y2);
};

//////////////////////////////////////////////////////////////////////////
class SkyScraper : public House
{
public:
   SkyScraper() : House() {}
   SkyScraper(SkyScraper & other) : House(other) {}

   virtual WorldObject * Clone();
   virtual void DoDraw(CDC3D & dc);
};

//////////////////////////////////////////////////////////////////////////
class OldCityHouse : public House
{
public:
   OldCityHouse() : House() {}
   OldCityHouse(OldCityHouse & other) : House(other) {}
   virtual WorldObject * Clone();
   virtual void DoDraw(CDC3D & dc);

};
#endif // House_h__
