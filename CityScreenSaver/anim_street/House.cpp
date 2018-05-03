#include "stdafx.h"
#include "House.h"
#include "AnimationWindowStreet.h"


double House::MAX_HOUSE_HEIGHT = 50;
double House::MAX_HOUSE_WIDTH = 50;
double House::WINDOW_HEIGHT = 10;
double House::WINDOW_WIDTH = 10;


House::House()
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
   //Windows
   if (detail == HighDetailLevel)
   {
      for (z = m_windowWidth; z <= houseWidth - m_windowWidth; z += m_windowWidth * 2)
      {
         //From bottom to top
         for (y = houseHeight - m_windowHeight*2; y >= m_windowHeight; y -= m_windowHeight * 2)
         {
            DrawLeftWallRect(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
         }
      }
   }

   //right wall
   DrawRightWallRect(dc, m_sideCol, 0, 0, houseWidth,  houseHeight);
   //Windows
   if (detail == HighDetailLevel)
   {
      for (z = m_windowWidth; z <= houseWidth - m_windowWidth; z += m_windowWidth * 2)
      {
         //From bottom to top
         for (y = houseHeight - m_windowHeight*2; y >= m_windowHeight; y -= m_windowHeight * 2)
         {
            DrawRightWallRect(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
         }
      }
   }

   //Front wall
   DrawFrontWallRect(dc, m_frontCol, 0, 0, houseWidth, houseHeight);
   if (detail != LowDetailLevel)
   {
      //Windows
      if (detail == HighDetailLevel)
      {
         for (x = m_windowWidth; x <= houseWidth - m_windowWidth; x += m_windowWidth * 2)
         {
            //From bottom to top
            for (y = houseHeight - m_windowHeight*2; y >= m_windowHeight; y -= m_windowHeight * 2)
            {
               DrawFrontWallRect(dc, m_windowCol, x, y, m_windowWidth, m_windowHeight);
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

//http://www.rapidtables.com/web/color/RGB_Color.htm
#define FRONT_COLOR_1 RGB(255, 165, 0),  //orange
#define FRONT_COLOR_1 RGB(255, 235, 205),//
#define FRONT_COLOR_1 RGB(107, 142, 35), //olive drab
#define FRONT_COLOR_1 RGB(64, 224, 208), //turquoise

#define SIDE_COLOR_1 RGB(255, 140, 0),  //dark orange
#define SIDE_COLOR_1 RGB(245, 222, 179),//
#define SIDE_COLOR_1 RGB(85, 107, 47),  //dark olive green
#define SIDE_COLOR_1 RGB(0, 206, 209),  //dark turquoise

#define COLORS_MAX (5)
static COLORREF st_frontColors[COLORS_MAX] =
{
   COLOR_CITY_SUNSET_1,
   COLOR_CITY_SUNSET_2,
   COLOR_CITY_SUNSET_3,
   COLOR_CITY_SUNSET_4,
   COLOR_CITY_SUNSET_5
};

static COLORREF st_sideColors[COLORS_MAX] =
{
   COLOR_CITY_SUNSET_SIDE_1,
   COLOR_CITY_SUNSET_SIDE_2,
   COLOR_CITY_SUNSET_SIDE_3,
   COLOR_CITY_SUNSET_SIDE_4,
   COLOR_CITY_SUNSET_SIDE_5
};

void House::GenerateColor(COLORREF & frontCol, COLORREF & sideCol)
{
   int i = RangedRandInt(0, COLORS_MAX-1);
   frontCol = st_frontColors[i];
   sideCol = st_sideColors[i];
}

void House::GenerateHouse(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth)
{
   //double h = RangedRand(houseHeight / 2, houseHeight * 2);
   //double h = RangedRandInt(1, MAX_FLOOR_NUMBER)*WINDOW_HEIGHT * 2 + WINDOW_HEIGHT;
   double h = RangedRand(WINDOW_HEIGHT * 3, MAX_HOUSE_HEIGHT);
   double w = RangedRand(MAX_HOUSE_WIDTH / 4, MAX_HOUSE_WIDTH);
   double d = RangedRand(cellDepth / 4, cellDepth);
   double xShift = RangedRand(0, cellWidth-w);
   SetPos(cellXPos + xShift, groundHeight - h, cellZPos);
   SetSize(w, h, d);
   m_windowHeight = WINDOW_HEIGHT;
   m_windowWidth = WINDOW_WIDTH;

   COLORREF frontCol, sideCol;
   House::GenerateColor(frontCol, sideCol);
   SetColor(frontCol, sideCol);
}

WorldObject * House::Clone()
{
   return new House(*this);
}

