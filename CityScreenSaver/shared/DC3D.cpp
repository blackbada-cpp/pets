#include "stdafx.h"
#include "DC3D.h"

//////////////////////////////////////////////////////////////////////////
// 0 <= cx_percent <= 100
// 0 <= cy_percent <= 100
CDC3D::CDC3D(RECT* pScreen, double z_camera, double planeWidth, double planeHeight, double cx_percent, double cy_percent)
   : m_detailOptimization(false)
{
   m_screen = *pScreen;
   //m_cy = planeWidth / 2;
   //m_cx = planeHeight / 2;
   m_cy = (pScreen->bottom - pScreen->top) / 2.0;
   m_cx = (pScreen->right - pScreen->left) / 2.0;

   m_projPlaneShiftX = (pScreen->right - pScreen->left) / 100.0 * cx_percent;
   m_projPlaneShiftY = (pScreen->bottom - pScreen->top) / 100.0 * cy_percent;

   m_focus = 2 * max(m_cx, m_cy);
   m_z_camera = z_camera;
}


CDC3D::~CDC3D()
{

}


//////////////////////////////////////////////////////////////////////////
// Parameters:
// z                  - depth coordinate
// x, return value px - are screen coordinates for CDC, go to the right
// O------------>
// |            X
// |
// |
// |
// |
// |
// |
// v Y
//
//
//    When looking from the top of monitor:
//    X---                                                               
//    |   ---                                                            
//    |      ---                        ZX    OF+OZ                      
//    |         ---                     --  = ----- =>  OX1 = ZX*OF / (OF + OZ)
//    |            -X1                  OX1    OF                        
//    |             | ---                                                
//    |             |    ---                                             
//    |             |       ---                                          
//    Z-------------O------------F - focus (eye)
//                  ^                                                           
//                  projection plane (monitor)
//
inline int CDC3D::ProjectionX(double x, double z)
{
   double OZ = z - m_z_camera;
   double OF = m_focus;
   double ZX = m_cx - (x - m_projPlaneShiftX); //m_cx - center of screen, m_projPlaneShiftX is controlled with WM_LEFT/WM_RIGHT keys
   if (0.0 < OF + OZ)
   {
      int OX1 = ZX*OF / (OF + OZ);
      int px = m_cx - OX1;
      return px;
   }
   else
   {
      if (ZX > 0)
         return m_screen.left;
      else
         return m_screen.right;
   }
}


int CDC3D::HorizonX()
{
   return m_cx;
}

//////////////////////////////////////////////////////////////////////////
// Parameters:
// z                  - depth coordinate
// y, return value py - are screen coordinates for CDC, go downstairs
// O------------>
// |            X
// |
// |
// |
// |
// |
// |
// v Y
//
//    When looking from the left side of monitor:
//    Y---                                                               
//    |   ---                                                            
//    |      ---                        ZY    OF+OZ                      
//    |         ---                     --  = ----- =>  OY1 = ZY*OF / (OF + OZ)
//    |            -Y1                  OY1    OF                        
//    |             | ---                                                
//    |             |    ---                                             
//    |             |       ---                                          
//    Z-------------O------------F - focus (eye)
//                  ^                                                           
//                  projection plane(monitor)
//
inline int CDC3D::ProjectionY(double y, double z)
{
   double OZ = z - m_z_camera;
   double OF = m_focus;
   double ZY = m_cy - (y - m_projPlaneShiftY); //m_cy - screen center, m_projPlaneShiftY is controlled with WM_UP/WM_DOWN keys
   if (0.0 < OF + OZ)
   {
      int OY1 = ZY*OF / (OF + OZ);
      int py = m_cy - OY1;
      return py;
   }
   else{
      if (ZY > 0)
         return m_screen.top;
      else
         return m_screen.bottom;
   }
}

int CDC3D::HorizonY()
{
   return m_cy;
}

void CDC3D::MoveTo(double x, double y, double z)
{
   int py = ProjectionY(y, z);
   int px = ProjectionX(x, z);

   CDC::MoveTo(px, py);
}

void CDC3D::MoveTo(POINT3D &p)
{
   MoveTo(p.x, p.y, p.z);
}

void CDC3D::LineTo(double x, double y, double z)
{
   int py = ProjectionY(y, z);
   int px = ProjectionX(x, z);
   CDC::LineTo(px, py);
}

void CDC3D::LineTo(POINT3D &p)
{
   LineTo(p.x, p.y, p.z);
}

bool CDC3D::IsPolygonFaceVisible(const POINT3D * lpPoints, int nCount, PolygonFace direction)
{
   if (direction == FaceAny)
      return true;

   bool visible = false;
   //Find 2 points with equal x and y, but different depth, z
   //Note: x and y follow screen coordinates:
   //
   // o-----> x
   // |
   // |
   // |
   // v y
   //z - is depth into the screen, distance from the screen into 3d world
   const POINT3D * p1 = &lpPoints[0];
   const POINT3D * p2 = NULL;
   for (int i = 1; i < nCount; i++)
   {
      if (p1->x == lpPoints[i].x && p1->y == lpPoints[i].y && p1->z != lpPoints[i].z)
      {
         p2 = &lpPoints[i];
         break;
      }
   }
   if (!p2) 
   {
      visible = true;
   }
   else 
   {
      const POINT3D * pNear = (p1->z < p2->z) ? p1 : p2;
      const POINT3D * pFar  = (p1->z > p2->z) ? p1 : p2;

      switch (direction)
      {
      case FaceToRight:  
         if (ProjectionX(pNear->x, pNear->z) < ProjectionX(pFar->x, pFar->z))
            visible = true;
         break;
      case FaceToLeft:   
         if (ProjectionX(pNear->x, pNear->z) > ProjectionX(pFar->x, pFar->z))
            visible = true;
         break;

      case FaceToTop:    
         //y goes to the bottom!
         //If near point is lower than far point => we see the face
         if (ProjectionY(pNear->y, pNear->z) > ProjectionY(pFar->y, pFar->z))
            visible = true;
         break;

      case FaceToBottom: 
         if (ProjectionY(pNear->y, pNear->z) < ProjectionY(pFar->y, pFar->z))
            visible = true;
         break;

      case FaceToFront:  visible = true; break;
      case FaceToBack:   visible = true; break;
      }
   }
   return visible;
}

BOOL CDC3D::Polygon(const POINT3D * lpPoints, int nCount, PolygonFace direction /*= Any*/)
{
   BOOL res = FALSE;
   if (IsPolygonFaceVisible(lpPoints, nCount, direction))
   {
      //--- >8 --- Allocate point array --- >8 ---
      static POINT st_points[32];
      POINT * projPoints = NULL;
      if (nCount > sizeof(st_points) / sizeof(st_points[0]))
      {
         projPoints = new POINT[nCount];
      }
      else
      {
         projPoints = st_points;
      }
      //--- >8 ---------------------------- >8 ---


      bool skip = false;
      for (int i = 0; i < nCount && !skip; i++)
      {
         //if (lpPoints[i].z < 0)
         //   skip = true;

         projPoints[i].x = ProjectionX(lpPoints[i].x, lpPoints[i].z);
         projPoints[i].y = ProjectionY(lpPoints[i].y, lpPoints[i].z);
      }
      if (!skip)
      {
         //CString msg;
         //msg.Format("Polygon(%d, %d) - (%d, %d) - (%d, %d) - (%d, %d)\n",
         //   projPoints[0].x, projPoints[0].y,
         //   projPoints[1].x, projPoints[1].y,
         //   projPoints[2].x, projPoints[2].y,
         //   projPoints[3].x, projPoints[3].y);
         //TRACE(msg);
         if (projPoints[0].x == projPoints[1].x && projPoints[0].y == projPoints[1].y)
         {
            projPoints[0].x--;
            //projPoints[1].y--;
         }
         res = CDC::Polygon(projPoints, nCount);
      }

      //--- >8 --- Deallocate point array --- >8 ---
      if (projPoints != st_points)
      {
         delete[] projPoints;
      }
      //--- >8 ------------------------------ >8 ---
   }
   return res;
}

void CDC3D::DrawBitmapObject(CBitmap & mask, CBitmap & bitmap, double x, double y, double width, double height, double z)
{
   int i;

   CDC dcMem;
   dcMem.CreateCompatibleDC(this);
   CBitmap * oldBitmap = dcMem.SelectObject(&mask);
   BITMAP bm;
   mask.GetBitmap(&bm);

   int py = ProjectionY(y, z);
   int px = ProjectionX(x, z);
   int py1 = ProjectionY(y + height, z);
   int px1 = ProjectionX(x + width, z);

   int pwidth = px1 - px;
   int pheight = py1 - py;


   //Cut black hole
   //dcMem.SelectObject(&m_mask);
   //dc.BitBlt(g_balls[i].x, g_balls[i].y, g_balls[i].width, g_balls[i].height, &dcMem, 0, 0, SRCAND);

   //dcMem.SelectObject(&m_ball);
   //dc.BitBlt(g_balls[i].x, g_balls[i].y, g_balls[i].width, g_balls[i].height, &dcMem, 0, 0, SRCPAINT);

   //Draw balls with stretching size
   //Cut black hole
   dcMem.SelectObject(&mask);
   StretchBlt(px, py, pwidth, pheight, &dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);

   dcMem.SelectObject(&bitmap);
   StretchBlt(px, py, pwidth, pheight, &dcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

   //Copy screen from buffer
   dcMem.SelectObject(oldBitmap);
}

void CDC3D::DrawBitmapObject(CBitmapObject & obj, double x, double y, double width, double height, double z)
{
   DrawBitmapObject(obj.m_mask, obj.m_bitmap, x, y, width, height, z);
}

DetailLevel CDC3D::GetDetailizationLevel(double z)
{
   double depth = z - m_z_camera;
   if (depth > m_lowDetailDepth)
      return LowDetailLevel;
   else if (depth > m_mediumDetailDepth)
      return MediumDetailLevel;
   return HighDetailLevel;
}

void CDC3D::InitDetailization(double mediumDetailDepth, double lowDetailDepth)
{
   ASSERT(lowDetailDepth > mediumDetailDepth);

   m_mediumDetailDepth = mediumDetailDepth;
   m_lowDetailDepth = lowDetailDepth;
}

//////////////////////////////////////////////////////////////////////////
void CreateBitmapMask(CBitmap & bmMask, CBitmap &bmColour, COLORREF crTransparent)
{
   CDC dcMem, dcMem2;
   HBITMAP hbmMask;
   BITMAP bm;

   bmColour.GetBitmap(&bm);
   bmMask.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

   dcMem.CreateCompatibleDC(NULL);
   dcMem2.CreateCompatibleDC(NULL);

   dcMem.SelectObject(&bmColour);
   dcMem2.SelectObject(&bmMask);

   dcMem.SetBkColor(crTransparent);

   dcMem2.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY);

   dcMem.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcMem2, 0, 0, SRCINVERT);
}


CBitmapObject::CBitmapObject(UINT nIDResource, COLORREF transparent)
{
   m_bitmap.LoadBitmap(nIDResource);
   CreateBitmapMask(m_mask, m_bitmap, transparent);
}
