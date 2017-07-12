#include "stdafx.h"
#include "House.h"



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
      for (z = m_windowWidth; z < houseWidth - m_windowWidth; z += m_windowWidth * 2)
      {
         for (y = m_windowHeight; y < houseHeight - m_windowHeight; y += m_windowHeight * 2)
         {
            DrawLeftWallRect(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
         }
      }
   }
   //DrawLeftWallRect(dc, m_windowCol, m_windowWidth,   m_windowHeight,   m_windowWidth, m_windowHeight);
   //DrawLeftWallRect(dc, m_windowCol, m_windowWidth*3, m_windowHeight,   m_windowWidth, m_windowHeight);
   //DrawLeftWallRect(dc, m_windowCol, m_windowWidth,   m_windowHeight*3, m_windowWidth, m_windowHeight);
   //DrawLeftWallRect(dc, m_windowCol, m_windowWidth*3, m_windowHeight*3, m_windowWidth, m_windowHeight);
   //DrawLeftWallRect(dc, m_windowCol, m_windowWidth,   m_windowHeight*5, m_windowWidth, m_windowHeight*2);//door
   //DrawLeftWallRect(dc, m_windowCol, m_windowWidth*3, m_windowHeight*5, m_windowWidth, m_windowHeight);

   //right wall
   DrawRightWallRect(dc, m_sideCol,   0,               0,                houseWidth,  houseHeight );
   //Windows
   if (detail == HighDetailLevel)
   {
      for (z = m_windowWidth; z < houseWidth - m_windowWidth; z += m_windowWidth * 2)
      {
         for (y = m_windowHeight; y < houseHeight - m_windowHeight; y += m_windowHeight * 2)
         {
            DrawRightWallRect(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
         }
      }
   }
   //DrawRightWallRect(dc, m_windowCol, m_windowWidth, m_windowHeight, m_windowWidth, m_windowHeight);
   //DrawRightWallRect(dc, m_windowCol, m_windowWidth * 3, m_windowHeight,     m_windowWidth, m_windowHeight);
   //DrawRightWallRect(dc, m_windowCol, m_windowWidth,     m_windowHeight * 3, m_windowWidth, m_windowHeight);
   //DrawRightWallRect(dc, m_windowCol, m_windowWidth * 3, m_windowHeight * 3, m_windowWidth, m_windowHeight);
   //DrawRightWallRect(dc, m_windowCol, m_windowWidth,     m_windowHeight * 5, m_windowWidth, m_windowHeight);
   //DrawRightWallRect(dc, m_windowCol, m_windowWidth * 3, m_windowHeight * 5, m_windowWidth, m_windowHeight*2);//door

   //Front wall
   if (detail != LowDetailLevel)
   {
      DrawFrontWallRect(dc, m_frontCol, 0, 0, houseWidth, houseHeight);
      //Windows
      if (detail == HighDetailLevel)
      {
         for (x = m_windowWidth; x < houseWidth - m_windowWidth; x += m_windowWidth * 2)
         {
            for (y = m_windowHeight; y < houseHeight - m_windowHeight; y += m_windowHeight * 2)
            {
               DrawFrontWallRect(dc, m_windowCol, x, y, m_windowWidth, m_windowHeight);
            }
         }
      }
   }
   //DrawFrontWallRect(dc, m_windowCol, m_windowWidth  , m_windowHeight  , m_windowWidth, m_windowHeight);
   //DrawFrontWallRect(dc, m_windowCol, m_windowWidth*3, m_windowHeight  , m_windowWidth, m_windowHeight);
   //DrawFrontWallRect(dc, m_windowCol, m_windowWidth  , m_windowHeight*3, m_windowWidth, m_windowHeight);
   //DrawFrontWallRect(dc, m_windowCol, m_windowWidth*3, m_windowHeight*3, m_windowWidth, m_windowHeight);
   //DrawFrontWallRect(dc, m_windowCol, m_windowWidth  , m_windowHeight*5, m_windowWidth, m_windowHeight);
   //DrawFrontWallRect(dc, m_windowCol, m_windowWidth*3, m_windowHeight*5, m_windowWidth, m_windowHeight);
   
   int wallWidth = 10;
   if (detail == HighDetailLevel)
      DrawRoof(dc, m_sideCol, 0, 0, houseWidth, houseWidth);
   DrawRoof(dc, m_roofCol, wallWidth, wallWidth, houseWidth - wallWidth - wallWidth, houseWidth - wallWidth - wallWidth);
}

LONG House::GetHouseHeight(RECT* prc)
{
   //Small houses:
   return (prc->bottom - prc->top) / 2 + (prc->bottom - prc->top) / 6;
   
   //Skyscrapers:
   //return (prc->bottom - prc->top) * 4;
}

LONG House::GetHouseWidth(RECT* prc)
{
   return (prc->right - prc->left) / 4;
}

LONG House::GetWindowWidth(RECT* prc)
{
   return GetHouseWidth(prc) / 5;
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
#define COLORS_MAX (4)
static COLORREF st_frontColors[COLORS_MAX] =
{
   RGB(255, 165, 0),  //orange
   RGB(255, 235, 205),//
   RGB(107, 142, 35), //olive drab
   RGB(64, 224, 208), //turquoise
};

static COLORREF st_sideColors[COLORS_MAX] =
{
   RGB(255, 140, 0),  //dark orange
   RGB(245, 222, 179),//
   RGB(85, 107, 47),  //dark olive green
   RGB(0, 206, 209),  //dark turquoise
};

static int RangedRand(int range_min, int range_max)
{
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   int u = (double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
   return u;
}

void House::GenerateColor(COLORREF & frontCol, COLORREF & sideCol)
{
   int i = RangedRand(0, COLORS_MAX-1);
   frontCol = st_frontColors[i];
   sideCol = st_sideColors[i];
}

