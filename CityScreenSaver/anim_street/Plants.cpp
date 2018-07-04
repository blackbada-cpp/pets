#include "stdafx.h"
#include "Plants.h"
#include "AnimationWindowStreet.h"


double Plant::PLANT_HEIGHT = 20.;
double Plant::PLANT_WIDTH  = 20.;

void Plant::DoDraw(CDC3D & dc)
{
   //Note: y is inverted, goes from top to bottom
 
   CPen     pen;   pen.CreatePen(PS_SOLID, 1, COLOR_FUJI_GREEN);
   CBrush   brush; brush.CreateSolidBrush(COLOR_FUJI_GREEN);

   CPen   * oldPen = dc.SelectObject(&pen);
   CBrush * oldBrush = dc.SelectObject(&brush);

   //tree crown
   double crownRadius = m_size.w;
   dc.Ellipse(m_pos, m_size.w, crownRadius*2.0);

   CPen     pen2;   pen2.CreatePen(PS_SOLID, 1, COLOR_FUJI_DEEP_RED);
   CBrush   brush2; brush2.CreateSolidBrush(COLOR_FUJI_DEEP_RED);

   dc.SelectObject(&pen2);
   dc.SelectObject(&brush2);

   //trunk
   POINT3D trunk[3];
   
   

   trunk[0].x = m_pos.x + m_size.w / 2.0;
   trunk[0].y = m_pos.y + crownRadius;
   trunk[0].z = m_pos.z;

   trunk[1].x = trunk[0].x + 5.;
   trunk[1].y = m_pos.y + m_size.h;
   trunk[1].z = m_pos.z;

   trunk[2].x = trunk[0].x - 5.;
   trunk[2].y = m_pos.y + m_size.h;
   trunk[2].z = m_pos.z;

   dc.Polygon(trunk, 3, CDC3D::FaceToFront);

   dc.SelectObject(oldPen);
   dc.SelectObject(oldBrush);

}

void Plant::Generate(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth)
{
//   double h = RangedRand(PLANT_HEIGHT / 2.0, PLANT_HEIGHT);
   double h = PLANT_HEIGHT;
   double w = PLANT_WIDTH;
   double d = PLANT_WIDTH;

   double xShift = RangedRand(0, cellWidth - w);
   double zShift = RangedRand(0, cellDepth - d);
   SetPos(cellXPos + xShift, groundHeight - h, cellZPos + zShift);
   SetSize(w, h, d);
}

