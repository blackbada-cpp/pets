#include "stdafx.h"
#include "House.h"
#include "AnimationWindowStreet.h"


double House::MAX_HOUSE_HEIGHT = 50;
double House::MAX_HOUSE_WIDTH = 50;
double House::WINDOW_HEIGHT = 10;
double House::WINDOW_WIDTH = 10;


House::House()
   : m_DoDraw_LeftWallVisible(false)
   , m_DoDraw_RightWallVisible(false)
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
, m_DoDraw_LeftWallVisible(false)
, m_DoDraw_RightWallVisible(false)
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
, m_DoDraw_LeftWallVisible(false)
, m_DoDraw_RightWallVisible(false)
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
, m_DoDraw_LeftWallVisible(false)
, m_DoDraw_RightWallVisible(false)
{
}


House::~House()
{
}

//WorldObject * House::Clone()
//{
//   return new House(*this);
//}

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

LONG House::GetPlantWidth(RECT* prc)
{
   return GetMaxHouseWidth(prc) / 6;
}

LONG House::GetPlantHeight(RECT* prc)
{
   return (prc->bottom - prc->top) / 2;
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

//////////////////////////////////////////////////////////////////////////
void House::DrawFrontWallPolygon(CDC3D & dc, COLORREF frontCol, int x, int y, int width, int height)
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

   dc.Polygon(wall, 4, CDC3D::FaceToFront);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

bool House::DrawLeftWallPolygon(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height)
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

   bool visible = !!dc.Polygon(wall, 4, CDC3D::FaceToLeft);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
   return visible;
}

bool House::DrawRightWallPolygon(CDC3D & dc, COLORREF frontCol, int z, int y, int width, int height)
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

   bool visible = !!dc.Polygon(wall, 4, CDC3D::FaceToRight);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
   return visible;
}

//////////////////////////////////////////////////////////////////////////
void House::DrawFrontWallLine(CDC3D & dc, COLORREF frontCol, int x1, int y1, int x2, int y2)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   wall[0].x = m_pos.x + x1;
   wall[0].y = m_pos.y + y1;
   wall[0].z = m_pos.z;

   wall[1].x = m_pos.x + x2;
   wall[1].y = m_pos.y + y2;
   wall[1].z = m_pos.z;

   //todo: CDC3D::FaceToFront
   //dc.Polyline(wall, 4, CDC3D::FaceToFront);
   dc.MoveTo(wall[0]);
   dc.LineTo(wall[1]);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

void House::DrawLeftWallLine(CDC3D & dc, COLORREF frontCol, int z1, int y1, int z2, int y2)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   wall[0].x = m_pos.x;
   wall[0].y = m_pos.y + y1;
   wall[0].z = m_pos.z + z1;

   wall[1].x = m_pos.x;
   wall[1].y = m_pos.y + y2;
   wall[1].z = m_pos.z + z2;

   //todo: CDC3D::FaceToLeft
   //dc.Polyline(wall, 4, CDC3D::FaceToLeft);
   dc.MoveTo(wall[0]);
   dc.LineTo(wall[1]);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

void House::DrawRightWallLine(CDC3D & dc, COLORREF frontCol, int z1, int y1, int z2, int y2)
{
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, frontCol);
   CBrush   brush; brush.CreateSolidBrush(frontCol);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);
   POINT3D wall[4];

   double houseWidth = m_size.w;

   //right bottom-front point
   wall[0].x = m_pos.x + houseWidth;
   wall[0].y = m_pos.y + y1;
   wall[0].z = m_pos.z + z1;

   //right bottom-back point
   wall[1].x = m_pos.x + houseWidth;
   wall[1].y = m_pos.y + y2;
   wall[1].z = m_pos.z + z2;

   //todo: CDC3D::FaceToRight
   //dc.Polyline(wall, 4, CDC3D::FaceToRight);
   dc.MoveTo(wall[0]);
   dc.LineTo(wall[1]);
   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);
}

//////////////////////////////////////////////////////////////////////////
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

void House::Generate(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth, int maxFloorNumber, std::vector<COLORREF> & frontColors, std::vector<COLORREF> & sideColors)
{
   //V1 Random size
   //double h = RangedRand(WINDOW_HEIGHT * 3, maxFloorNumber*WINDOW_HEIGHT * 2 + WINDOW_HEIGHT);
   //double w = RangedRand(WINDOW_WIDTH * 4, MAX_HOUSE_WIDTH);
   //double d = RangedRand(WINDOW_WIDTH * 4, MAX_HOUSE_WIDTH);
   
   //V2 Random # of floors
   int floorNumber = RangedRandInt(1, maxFloorNumber);
   double h = floorNumber*WINDOW_HEIGHT * 2 + WINDOW_HEIGHT;

   //V2 Random # of windows
   int orientation = RangedRandInt(0, 1);
   int windowsNumber1 = RangedRandInt(2, 3);
   int windowsNumber2 = RangedRandInt(3, 6);
   double size1 = windowsNumber1*WINDOW_WIDTH * 2 + WINDOW_WIDTH;
   double size2 = windowsNumber2*WINDOW_WIDTH * 2 + WINDOW_WIDTH;
   double w = orientation ? size1 : size2;
   double d = orientation ? size2 : size1;

   double xShift = RangedRand(0, cellWidth - w);
   double zShift = RangedRand(0, cellDepth - d);
   SetPos(cellXPos + xShift, groundHeight - h, cellZPos + zShift);
   SetSize(w, h, d);
   m_windowHeight = WINDOW_HEIGHT;
   m_windowWidth = WINDOW_WIDTH;

   GenerateColor(frontColors, sideColors);
}

void House::DoDraw(CDC3D & dc)
{
   m_DoDraw_LeftWallVisible  = false;
   m_DoDraw_RightWallVisible = false;
   //WARNING: Y goes from the top to bottom
   int houseHeight = m_size.h;
   int houseWidth = m_size.w;
   int houseDepth = m_size.d;

   DetailLevel detail = dc.GetDetailizationLevel(m_pos.z);

   //Left wall
   m_DoDraw_LeftWallVisible = DrawLeftWallPolygon(dc, m_sideCol, 0, 0, houseDepth, houseHeight);

   //right wall
   m_DoDraw_RightWallVisible = DrawRightWallPolygon(dc, m_sideCol, 0, 0, houseDepth, houseHeight);

   //Front wall
   DrawFrontWallPolygon(dc, m_frontCol, 0, 0, houseWidth, houseHeight);

   int roofBorderWidth = 10;
   if (detail == HighDetailLevel)
   {
      DrawRoof(dc, m_sideCol, 0, 0, houseWidth, houseDepth);
      DrawRoof(dc, m_roofCol, roofBorderWidth, roofBorderWidth, houseWidth - roofBorderWidth * 2, houseDepth - roofBorderWidth * 2);
   }
   else
   {
      DrawRoof(dc, m_roofCol, 0, 0, houseWidth, houseDepth);
   }
}

//////////////////////////////////////////////////////////////////////////
WorldObject * SkyScraper::Clone()
{
   return new SkyScraper(*this);
}

void SkyScraper::DoDraw(CDC3D & dc)
{
   //WARNING: Y goes from the top to bottom
   //Calculates m_DoDraw_LeftWallVisible and m_DoDraw_RightWallVisible
   House::DoDraw(dc);

   int houseHeight = m_size.h;
   int houseWidth = m_size.w;
   int houseDepth = m_size.d;
   int x, y, z;
   DetailLevel detail = dc.GetDetailizationLevel(m_pos.z);

   //Left wall Windows
   if (m_DoDraw_LeftWallVisible && detail == HighDetailLevel)
   {
      //Vertical lines to the depth
      for (z = 0; z < houseDepth; z += m_windowWidth * 4)
      {
         DrawLeftWallLine(dc, m_windowCol, z, 0, z, houseHeight);
      }
      DrawLeftWallLine(dc, m_windowCol, houseDepth, 0, houseDepth, houseHeight);

      //Horizontal lines from bottom to top
      for (y = houseHeight; y > 0; y -= m_windowHeight * 4)
      {
         DrawLeftWallLine(dc, m_windowCol, 0, y, houseDepth, y);
      }
      DrawLeftWallLine(dc, m_windowCol, 0, 0/*y1*/, houseDepth, 0/*y2*/);
   }

   //right wall Windows
   if (m_DoDraw_RightWallVisible && detail == HighDetailLevel)
   {
      //Vertical lines to the depth
      for (z = 0; z < houseDepth; z += m_windowWidth * 4)
      {
         DrawRightWallLine(dc, m_windowCol, z, 0, z, houseHeight);
      }
      DrawRightWallLine(dc, m_windowCol, houseDepth, 0, houseDepth, houseHeight);

      //Horizontal lines from bottom to top
      for (y = houseHeight; y > 0; y -= m_windowHeight * 4)
      {
         DrawRightWallLine(dc, m_windowCol, 0, y, houseDepth, y);
      }
      DrawRightWallLine(dc, m_windowCol, 0, 0/*y1*/, houseDepth, 0/*y2*/);
   }

   //Front wall windows
   if (detail == HighDetailLevel)
   {
      //Vertical lines from left to right
      for (x = 0; x < houseWidth; x += m_windowWidth * 4)
      {
         DrawFrontWallLine(dc, m_windowCol, x, 0, x, houseHeight);
      }
      DrawFrontWallLine(dc, m_windowCol, houseWidth, 0, houseWidth, houseHeight);

      //Horizontal lines from bottom to top
      for (y = houseHeight; y > 0; y -= m_windowHeight * 4)
      //for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 4)
      {
         DrawFrontWallLine(dc, m_windowCol, 0, y, houseWidth, y);
      }
      DrawFrontWallLine(dc, m_windowCol, 0, 0/*y1*/, houseWidth, 0/*y2*/);
   }
}

//////////////////////////////////////////////////////////////////////////
WorldObject * OldCityHouse::Clone()
{
   return new OldCityHouse(*this);
}

void OldCityHouse::DoDraw(CDC3D & dc)
{
   //WARNING: Y goes from the top to bottom
   //Calculates m_DoDraw_LeftWallVisible and m_DoDraw_RightWallVisible
   House::DoDraw(dc);
   
   int houseHeight = m_size.h;
   int houseWidth = m_size.w;
   int houseDepth = m_size.d;
   int x, y, z;
   DetailLevel detail = dc.GetDetailizationLevel(m_pos.z);

   //Left wall Windows
   if (m_DoDraw_LeftWallVisible && detail == HighDetailLevel)
   {
      for (z = m_windowWidth; z <= houseDepth - m_windowWidth; z += m_windowWidth * 2)
      {
         //From bottom to top
         for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 2)
         {
            DrawLeftWallPolygon(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
         }
      }
   }

   //right wall Windows
   if (m_DoDraw_RightWallVisible && detail == HighDetailLevel)
   {
      for (z = m_windowWidth; z <= houseDepth - m_windowWidth; z += m_windowWidth * 2)
      {
         //From bottom to top
         for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 2)
         {
            DrawRightWallPolygon(dc, m_windowCol, z, y, m_windowWidth, m_windowHeight);
         }
      }
   }

   //Front wall windows
   if (detail == HighDetailLevel)
   {
      for (x = m_windowWidth; x <= houseWidth - m_windowWidth; x += m_windowWidth * 2)
      {
         //From bottom to top
         for (y = houseHeight - m_windowHeight * 2; y >= m_windowHeight; y -= m_windowHeight * 2)
         {
            DrawFrontWallPolygon(dc, m_windowCol, x, y, m_windowWidth, m_windowHeight);
         }
      }
   }
}
