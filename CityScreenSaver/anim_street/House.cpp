#include "stdafx.h"
#include "House.h"
#include "AnimationWindowStreet.h"


double House::MAX_HOUSE_HEIGHT = 50;
double House::MAX_HOUSE_WIDTH = 50;
double House::WINDOW_HEIGHT = 10;
double House::WINDOW_WIDTH = 10;


House::House(HouseStyle style)
   : m_style(style)
{
   m_frontCol = RGB(255, 235, 205);
   m_sideCol = RGB(245, 222, 179);
   m_windowCol = RGB(128, 128, 128);
   m_roofCol = RGB(50, 50, 50);
   m_windowHeight = 10;
   m_windowWidth = 10;
}

House::House(POINT3D &pos, SIZE3D &size)
: WorldObject(pos, size)
, m_style(OldTown)
{
   m_frontCol = RGB(255, 235, 205);
   m_sideCol = RGB(245, 222, 179);
   m_windowCol = RGB(128, 128, 128);
   m_roofCol = RGB(50, 50, 50);
   m_windowHeight = 10;
   m_windowWidth = 10;
}

House::House(int x, int y, int z, int w, int h, int d)
: WorldObject(x, y, z, w, h, d)
, m_style(OldTown)
{
   m_frontCol  = RGB(255, 235, 205);
   m_sideCol   = RGB(245, 222, 179);
   m_windowCol = RGB(128, 128, 128);
   m_roofCol = RGB(50, 50, 50);
   m_windowHeight = 10;
   m_windowWidth = 10;
}

House::House(House & other)
: WorldObject(other)
, m_frontCol    (other.m_frontCol    )
, m_sideCol     (other.m_sideCol     )
, m_windowCol   (other.m_windowCol   )
, m_roofCol     (other.m_roofCol     )
, m_windowHeight(other.m_windowHeight)
, m_windowWidth (other.m_windowWidth )
, m_style       (other.m_style)
{
}


House::~House()
{
}

void House::DoDraw(CDC3D & dc)
{
   CPen     penSide;    penSide.   CreatePen(PS_SOLID, 1, m_sideCol);
   CBrush   brushSide;  brushSide. CreateSolidBrush(m_sideCol);
   int houseHeight  = m_size.h;
   int houseWidth   = m_size.w;
   int x, y, z;

   DetailLevel detail = dc.GetDetailizationLevel(m_pos.z);
   
   //Left wall
   DrawLeftWallRect(dc, m_sideCol, 0, 0, houseWidth, houseHeight);
   if (m_style != SkyScraper)
   {
      //Windows
      if (detail == HighDetailLevel)
      {
         for (z = m_windowWidth; z <= houseWidth - m_windowWidth; z += m_windowWidth * 2)
         {
            //From bottom to top
            for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 2)
            {
               DrawLeftWallRect(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
            }
         }
      }
   }

   //right wall
   DrawRightWallRect(dc, m_sideCol, 0, 0, houseWidth,  houseHeight);
   
   if (m_style != SkyScraper)
   {
      //Windows
      if (detail == HighDetailLevel)
      {
         for (z = m_windowWidth; z <= houseWidth - m_windowWidth; z += m_windowWidth * 2)
         {
            //From bottom to top
            for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 2)
            {
               DrawRightWallRect(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
            }
         }
      }
   }

   //Front wall
   DrawFrontWallRect(dc, m_frontCol, 0, 0, houseWidth, houseHeight);
   
   if (m_style != SkyScraper)
   {
      if (detail != LowDetailLevel)
      {
         //Windows
         if (detail == HighDetailLevel)
         {
            for (x = m_windowWidth; x <= houseWidth - m_windowWidth; x += m_windowWidth * 2)
            {
               //From bottom to top
               for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 2)
               {
                  DrawFrontWallRect(dc, m_windowCol, x, y, m_windowWidth, m_windowHeight);
               }
            }
         }
      }
   }
   
   int wallWidth = 10;
   if (detail == HighDetailLevel)
      DrawRoof(dc, m_sideCol, 0, 0, houseWidth, houseWidth);
   DrawRoof(dc, m_roofCol, wallWidth, wallWidth, houseWidth - wallWidth - wallWidth, houseWidth - wallWidth - wallWidth);
}

LONG House::GetMaxHouseHeight(RECT* prc)
{
   //Small houses:
   //return (prc->bottom - prc->top) / 2 + (prc->bottom - prc->top) / 6;
   return (prc->bottom - prc->top)*2;
   
   //Skyscrapers:
   //return (prc->bottom - prc->top) * 4;
}

LONG House::GetMaxHouseWidth(RECT* prc)
{
   return (prc->right - prc->left) ;
}

LONG House::GetWindowWidth(RECT* prc)
{
   return GetMaxHouseWidth(prc) / 12;
}

LONG House::GetWindowHeight(RECT* prc)
{
   return ((prc->bottom - prc->top) / 2 + (prc->bottom - prc->top) / 6) / 7;
   //TODO: return GetHouseHeight(prc) / 7;
}

void House::DrawRoof(CDC3D & dc, COLORREF frontCol, int x, int z, int width, int depth)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   double houseHeight = 0.0;// m_size.h;

   //left top-front point
   wall[0].x = m_pos.x + x;
   wall[0].y = m_pos.y + houseHeight;
   wall[0].z = m_pos.z + z;
   
   //left top-back point
   wall[1].x = m_pos.x + x;
   wall[1].y = m_pos.y + houseHeight;
   wall[1].z = m_pos.z + z + depth;

   //right top-back point
   wall[2].x = m_pos.x + x + width;
   wall[2].y = m_pos.y + houseHeight;
   wall[2].z = m_pos.z + z + depth;

   //right top-front point
   wall[3].x = m_pos.x + x + width;
   wall[3].y = m_pos.y + houseHeight;
   wall[3].z = m_pos.z + z;

   dc.Polygon(wall, 4, CDC3D::FaceToTop);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

void House::DrawFrontWallRect(CDC3D & dc, COLORREF frontCol, int x, int y, int width, int height)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   wall[0].x = m_pos.x+x;
   wall[0].y = m_pos.y+y;
   wall[0].z = m_pos.z;

   wall[1].x = m_pos.x+x + width;
   wall[1].y = m_pos.y+y;
   wall[1].z = m_pos.z;

   wall[2].x = m_pos.x+x + width;
   wall[2].y = m_pos.y+y + height;
   wall[2].z = m_pos.z;

   wall[3].x = m_pos.x+x;
   wall[3].y = m_pos.y+y + height;
   wall[3].z = m_pos.z;

   dc.Polygon(wall, 4);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

void House::DrawLeftWallRect(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   //left bottom-front point
   wall[0].x = m_pos.x;
   wall[0].y = m_pos.y+y;
   wall[0].z = m_pos.z+z;

   //left bottom-back point
   wall[1].x = m_pos.x;
   wall[1].y = m_pos.y+y;
   wall[1].z = m_pos.z+z + width;

   //left top-back point
   wall[2].x = m_pos.x;
   wall[2].y = m_pos.y+y + height;
   wall[2].z = m_pos.z+z + width;

   //left top-front point
   wall[3].x = m_pos.x;
   wall[3].y = m_pos.y+y + height;
   wall[3].z = m_pos.z+z;

   dc.Polygon(wall, 4);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

void House::DrawRightWallRect(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   double houseWidth = m_size.w;
   
   //right bottom-front point
   wall[0].x = m_pos.x + houseWidth;
   wall[0].y = m_pos.y + y;
   wall[0].z = m_pos.z + z;

   //right bottom-back point
   wall[1].x = m_pos.x + houseWidth;
   wall[1].y = m_pos.y + y;
   wall[1].z = m_pos.z + z + width;

   //right top-back point
   wall[2].x = m_pos.x + houseWidth;
   wall[2].y = m_pos.y + y + height;
   wall[2].z = m_pos.z + z + width;

   //right top-front point
   wall[3].x = m_pos.x + houseWidth;
   wall[3].y = m_pos.y + y + height;
   wall[3].z = m_pos.z + z;

   dc.Polygon(wall, 4);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

void House::SetColor(COLORREF frontCol, COLORREF sideCol)
{
   m_frontCol = frontCol; m_sideCol = sideCol;
}



void House::GenerateColor(std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors)
{
   assert(frontColors.size() == sideColors.size());
   
   int i = RangedRandInt(0, frontColors.size() - 1);
   COLORREF  frontCol = frontColors[i];
   COLORREF  sideCol = sideColors[i];
   SetColor(frontCol, sideCol);
}

void House::GenerateHouse(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth, int maxFloorNumber, std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors)
{
   //double h = RangedRand(houseHeight / 2, houseHeight * 2);
   //double h = RangedRandInt(1, MAX_FLOOR_NUMBER)*WINDOW_HEIGHT * 2 + WINDOW_HEIGHT;
   double h = RangedRand(WINDOW_HEIGHT * 3, maxFloorNumber*WINDOW_HEIGHT * 2 + WINDOW_HEIGHT);
   double w = RangedRand(MAX_HOUSE_WIDTH / 4, MAX_HOUSE_WIDTH);
   double d = RangedRand(MAX_HOUSE_WIDTH / 4, MAX_HOUSE_WIDTH);
   double xShift = RangedRand(0, cellWidth - w);
   double zShift = RangedRand(0, cellDepth - d);
   SetPos(cellXPos + xShift, groundHeight - h, cellZPos + zShift);
   SetSize(w, h, d);
   m_windowHeight = WINDOW_HEIGHT;
   m_windowWidth = WINDOW_WIDTH;

   GenerateColor(frontColors, sideColors);
}

WorldObject * House::Clone()
{
   return new House(*this);
}

