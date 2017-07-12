#ifndef DC3D_h__
#define DC3D_h__

#include <afxwin.h>

struct POINT3D {
   double x;
   double y;
   double z;

   POINT3D()
   {
      x = 0;
      y = 0;
      z = 0;
   }
   POINT3D(double _x, double _y, double _z)
   {
      x = _x;
      y = _y;
      z = _z;
   }
   POINT3D(int _x, int _y, int _z)
   {
      x = (double)_x;
      y = (double)_y;
      z = (double)_z;
   }
};

struct SIZE3D {
   double w;//width
   double h;//heitht
   double d;//depth

   SIZE3D()
   {
      w = 0;
      h = 0;
      d = 0;
   }
   SIZE3D(double _w, double _h, double _d)
   {
      w = _w;
      h = _h;
      d = _d;
   }
   SIZE3D(int _w, int _h, int _d)
   {
      w = (double)_w;
      h = (double)_h;
      d = (double)_d;
   }
};

//////////////////////////////////////////////////////////////////////////
class CBitmapObject
{
public:
   CBitmap m_bitmap;
   CBitmap m_mask;

   CBitmapObject(UINT nIDResource, COLORREF transparent = RGB(0, 0, 0));
};

enum DetailLevel
{
   HighDetailLevel,
   MediumDetailLevel,
   LowDetailLevel
};
//////////////////////////////////////////////////////////////////////////
// Performs 3D-drawing
//////////////////////////////////////////////////////////////////////////
class CDC3D : public CDC
{
public:
   
protected:
   CRect m_screen;
   double m_cy;      //screen x center
   double m_cx;      //screen y center
   double m_projPlaneShiftY;      //screen x center
   double m_projPlaneShiftX;      //screen y center
   double m_focus;   //distance between eye and projection screen
   double m_z_camera;//z coordinate of projection plane
                     // for animation when we want to fly through objects by moving the camera (increasing m_z_camera)
   
   //Optimization of drawing far distance objects
   bool   m_detailOptimization;
   double m_mediumDetailDepth;
   double m_lowDetailDepth;

public:
   CDC3D(RECT* pScreen, double z_camera, double planeWidth, double planeHeight, double cx_percent, double cy_percent);
   virtual ~CDC3D();
   inline int ProjectionX(double x, double z); //projection of x coord, 
   inline int ProjectionY(double y, double z); //projection of y coord
   int HorizonX(); //projection of x coord, 
   int HorizonY(); //projection of y coord
   double GetEyeZ() { return m_z_camera - m_focus + 0.1;  }
   void        InitDetailization(double mediumDetailDepth, double lowDetailDepth);
   DetailLevel GetDetailizationLevel(double z); //projection of y coord

   void MoveTo(double x, double y, double z);
   void MoveTo(POINT3D &p);

   void LineTo(double x, double y, double z);
   void LineTo(POINT3D &p);

   //Draws bitmap, using mask to clean destination bitmap
   void DrawBitmapObject(CBitmap & mask, CBitmap & bitmap, double x, double y, double width, double height, double z);
   void DrawBitmapObject(CBitmapObject & obj, double x, double y, double width, double height, double z); 

   //Sort of normal
   enum PolygonFace
   {
      FaceAny,
      FaceToRight,
      FaceToLeft,
      FaceToTop,
      FaceToBottom,
      FaceToFront,
      FaceToBack,
   };

   BOOL Polygon(const POINT3D * lpPoints, int nCount, PolygonFace direction = FaceAny);
   bool IsPolygonFaceVisible(const POINT3D * lpPoints, int nCount, PolygonFace direction);
};

#endif // DC3D_h__