
// MFCApplication1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "AnimationWindowStreet.h"
#include "afxdialogex.h"
#include "DC3D.h"
#include "World.h"
#include "House.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int ID_TIMER = 1;


LONG GetGroundHeight(RECT* prc)
{
   return prc->bottom / 2 + prc->bottom / 6;
}

//////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
// CAnimationWindowStreet dialog



CAnimationWindowStreet::CAnimationWindowStreet(CWnd* pParent /*=NULL*/)
: m_city()
, m_road(HOUSE_COUNT * m_z_houseStep, m_z_houseStep)
, m_pressed(key_Nothing)
, m_cxPercent(X_INIT_PERCENT)
, m_cyPercent(Y_INIT_PERCENT)
, m_cameraSpeed(CAMERA_SPEED)
, m_z_camera(Z_CAMERA_INIT)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_planeWidth = 100.0;
   m_planeHeight = 100.0;
}

CAnimationWindowStreet::~CAnimationWindowStreet()
{
   //m_world.clear();
}


BEGIN_MESSAGE_MAP(CAnimationWindowStreet, CWnd)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_WM_TIMER()
   ON_WM_DESTROY()
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
END_MESSAGE_MAP()



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAnimationWindowStreet::OnPaint()
{
	if (IsIconic())
	{
      CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
      //CWnd::OnPaint();
      
      CPaintDC dc(this); // device context for painting

      CRect rc;
      GetClientRect(rc);

      CBitmap bmp;
      bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

      //Moving houses and balls
      //CDC3D mdc(&rc, 0 /*m_z_camera*/, m_planeWidth, m_planeHeight, m_cxPercent, m_cyPercent);
      //Moving camera
      CDC3D mdc(&rc, m_z_camera, m_planeWidth, m_planeHeight, m_cxPercent, m_cyPercent);
      mdc.InitDetailization(m_z_houseStep * DETAILIZATION_MEDIUM, m_z_houseStep * DETAILIZATION_LOW);
      mdc.CreateCompatibleDC(&dc);
      CBitmap* oldBitmap = mdc.SelectObject(&bmp);

      rc = dc.m_ps.rcPaint;

      DrawStreet(mdc, &rc);
      
      dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(),
         &mdc, rc.left, rc.top, SRCCOPY);
      mdc.SelectObject(oldBitmap);
   }
}

int CAnimationWindowStreet::OnCreate(LPCREATESTRUCT cs)
{
   UINT ret;

   ret = SetTimer(ID_TIMER, 5, NULL);
   if (ret == 0)
      MessageBox("Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
   
   CRect rc;
   GetClientRect(rc);
   
   m_planeWidth  = rc.Width();
   m_planeHeight = rc.Height();


#ifdef MOVE_CAMERA
   m_z_camera_limit = m_z_houseStep*HOUSE_COUNT;
#endif
   m_city.Init(rc, HOUSE_COUNT, m_z_houseStep, ::GetGroundHeight(&rc));
   //Duplicate city
   m_city2.CopyFrom(m_city);
   m_city2.MoveObjects(m_city.GetCityDepth());
   
   m_road.Init(rc);

   return CWnd::OnCreate(cs);
}


void CAnimationWindowStreet::OnDestroy()
{
   KillTimer(ID_TIMER);

   //World::reverse_iterator it;
   //for (it = m_world.rbegin(); it != m_world.rend(); it++)
   //{
   //   WorldObject * obj = (*it).second;
   //   delete obj;
   //}

   PostQuitMessage(0);
}

void CAnimationWindowStreet::OnTimer(UINT_PTR)
{
   static clock_t;

   switch (m_pressed)
   {
   case key_Nothing:
      break;
   case key_Left:
      if (m_cxPercent > X_MIN_PERCENT ) m_cxPercent -= D_PERCENT;
      break;
   case key_Right:
      if (m_cxPercent < X_MAX_PERCENT) m_cxPercent += D_PERCENT;
      break;
   case key_Up:
      if (m_cyPercent > Y_MIN_PERCENT) m_cyPercent -= D_PERCENT;
      break;
   case key_Down:
      if (m_cyPercent < Y_MAX_PERCENT) m_cyPercent += D_PERCENT;
      break;
   case key_Plus:
      if (m_cameraSpeed < CAMERA_SPEED_MAX) m_cameraSpeed += CAMERA_SPEED_INCREASE;
      break;
   case key_Minus:
      if (m_cameraSpeed > CAMERA_SPEED_MIN) m_cameraSpeed -= CAMERA_SPEED_INCREASE;
      break;
   default:
      break;
   }

#ifdef MOVE_CAMERA
   m_z_camera = m_z_camera + m_cameraSpeed;
   if (m_z_camera > m_z_camera_limit)
      m_z_camera = Z_CAMERA_INIT;
#endif
   //Move camera forever:
   //m_z_camera += m_cameraSpeed;
   //flip camera back for tonnel: if (m_z_camera > m_z_houseStep)
   //flip camera back for tonnel:    m_z_camera = 0.0;
   RECT rcClient;
   GetClientRect(&rcClient);

#ifndef MOVE_CAMERA
   //Move objects instead of camera
   m_city.MoveObjects(m_cameraSpeed, m_z_houseStep, m_z_camera);
   m_road.MoveForward(&rcClient, m_cameraSpeed, m_z_camera);
#endif
   Invalidate();
}

void CAnimationWindowStreet::DrawStreet(CDC3D & dc, RECT* prc)
{
   //CBrush brush; brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   //http://www.rapidtables.com/web/color/RGB_Color.htm
   COLORREF colNight = RGB(0, 0, 205);
   COLORREF colDay = RGB(30, 144, 255);// RGB(0, 191, 255);
   COLORREF colGrass = COLOR_GRASSS_MEADOW;// COLOR_GRASSS_BLADE_OF_GRASS;
   CBrush brushDay; brushDay.CreateSolidBrush(colDay);
   CBrush brushLawn; brushLawn.CreateSolidBrush(colGrass);

   //Draw Background
   //int horizon = dc.ProjectionY(GetGroundHeight(prc), m_tonnelDepthEnd);
   int horizon = dc.HorizonY();

   CRect rSky(prc->left, prc->top, prc->right, horizon);
   CRect rGround(prc->left, horizon, prc->right, prc->bottom);
   dc.FillRect(rSky, &brushDay);
   dc.FillRect(rGround, &brushLawn);

   //Draw Road infrastructure
   m_road.Draw(dc, prc);

   //Draw 3D Objects
   World world;
   m_city2.PrepareDraw(world);
   m_city.PrepareDraw(world);

   world.DoDraw(dc);

}


void CAnimationWindowStreet::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);
   Invalidate();
}


void CAnimationWindowStreet::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
   if (m_pressed == key_Nothing)
   {
      if      (nChar == VK_LEFT)  m_pressed = key_Left;
      else if (nChar == VK_RIGHT) m_pressed = key_Right;
      else if (nChar == VK_UP)    m_pressed = key_Up;
      else if (nChar == VK_DOWN)  m_pressed = key_Down;
      else if (nChar == VK_ADD)      m_pressed = key_Plus;
      else if (nChar == VK_SUBTRACT)      m_pressed = key_Minus;
   }
}

void CAnimationWindowStreet::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
   m_pressed = key_Nothing;
}

//////////////////////////////////////////////////////////////////////////
City::City()
: m_cellCount(0)
, m_cellDepth(0.0)
{
   srand((unsigned)time(NULL));
}

City::~City()
{
   DestroyRow(m_balls);
   DestroyRow(m_leftRow1);
   DestroyRow(m_leftRow2);
   DestroyRow(m_leftRow3);
   DestroyRow(m_rightRow1);
   DestroyRow(m_rightRow2);
   DestroyRow(m_rightRow3);
}

void City::CopyFrom(City &other)
{
   m_cellCount = other.m_cellCount;
   m_cellDepth = other.m_cellDepth;

   CopyRow(m_balls    , other.m_balls    );
   CopyRow(m_leftRow1 , other.m_leftRow1 );
   CopyRow(m_leftRow2 , other.m_leftRow2 );
   CopyRow(m_leftRow3 , other.m_leftRow3 );
   CopyRow(m_rightRow1, other.m_rightRow1);
   CopyRow(m_rightRow2, other.m_rightRow2);
   CopyRow(m_rightRow3, other.m_rightRow3);
}

void City::DestroyRow(std::vector<WorldObject*> &row)
{
   for (auto it = row.begin(); it != row.end(); ++it)
   {
      WorldObject* obj = *it;
      if (obj != NULL)
         delete obj;
      *it = NULL;
   }
}

double RangedRand(int range_min, int range_max)
{
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   double u = (double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
   return u;
}
int RangedRandInt(int range_min, int range_max)
{
   return (int)RangedRand(range_min, range_max);
}

void City::Init(CRect & rc, int cellCount, double cellDepth, double groundHeight)
{
   m_cellCount = cellCount;
   m_cellDepth = cellDepth;

   //////////////////////////////////////////////////////////////////////////
   House::MAX_HOUSE_HEIGHT = House::GetMaxHouseHeight(&rc);
   House::MAX_HOUSE_WIDTH = House::GetMaxHouseWidth(&rc);
   House::WINDOW_HEIGHT = House::GetWindowHeight(&rc);
   House::WINDOW_WIDTH = House::GetWindowWidth(&rc);

   int i;
   //////////////////////////////////////////////////////////////////////////
   // Init random balls
   for (int i = 0; i < BALL_COUNT; i++)
   {
      Ball * ball = new Ball();
      ball -> SetPos(RangedRand(0, rc.Width()), groundHeight, RangedRand(0, GetCityDepth()));
      m_balls.push_back(ball);
   }

   //////////////////////////////////////////////////////////////////////////
   m_rightRow1.assign(m_cellCount, NULL);
   m_rightRow2.assign(m_cellCount, NULL);
   m_rightRow3.assign(m_cellCount, NULL);
   m_leftRow1.assign(m_cellCount, NULL);
   m_leftRow2.assign(m_cellCount, NULL);
   m_leftRow3.assign(m_cellCount, NULL);

   //Create side roads
   //Create extra objects
   //Create houses
   CreateRow(m_leftRow1,  0, (int)(m_cellCount*0.5), 50);
   CreateRow(m_leftRow2,  0, (int)(m_cellCount*0.5), 40);
   CreateRow(m_leftRow3,  0, (int)(m_cellCount*0.5), 10);
   CreateRow(m_rightRow1, 0, (int)(m_cellCount*0.5), 50);
   CreateRow(m_rightRow2, 0, (int)(m_cellCount*0.5), 40);
   CreateRow(m_rightRow3, 0, (int)(m_cellCount*0.5), 10);
   CreateRow(m_leftRow1,  (int)(m_cellCount*0.5), (int)(m_cellCount*0.7), 30);
   CreateRow(m_leftRow2,  (int)(m_cellCount*0.5), (int)(m_cellCount*0.7), 10);
   CreateRow(m_leftRow3,  (int)(m_cellCount*0.5), (int)(m_cellCount*0.7), 5);
   CreateRow(m_rightRow1, (int)(m_cellCount*0.5), (int)(m_cellCount*0.7), 30);
   CreateRow(m_rightRow2, (int)(m_cellCount*0.5), (int)(m_cellCount*0.7), 10);
   CreateRow(m_rightRow3, (int)(m_cellCount*0.5), (int)(m_cellCount*0.7), 5);

   //////////////////////////////////////////////////////////////////////////
   //Init houses
   double widthLimit = rc.right;
   InitCellRow(m_rightRow1, rc.right                             , groundHeight, 2*House::MAX_HOUSE_WIDTH, cellDepth);
   InitCellRow(m_rightRow2, rc.right + 2 * House::MAX_HOUSE_WIDTH, groundHeight, 2*House::MAX_HOUSE_WIDTH, cellDepth);
   InitCellRow(m_rightRow3, rc.right + 4 * House::MAX_HOUSE_WIDTH, groundHeight, 2*House::MAX_HOUSE_WIDTH, cellDepth);
   InitCellRow(m_leftRow1,  0 - 2 * House::MAX_HOUSE_WIDTH       , groundHeight, 2*House::MAX_HOUSE_WIDTH, cellDepth);
   InitCellRow(m_leftRow2,  0 - 4 * House::MAX_HOUSE_WIDTH       , groundHeight, 2*House::MAX_HOUSE_WIDTH, cellDepth);
   InitCellRow(m_leftRow3,  0 - 6 * House::MAX_HOUSE_WIDTH       , groundHeight, 2*House::MAX_HOUSE_WIDTH, cellDepth);
}

void City::CopyRow(std::vector<WorldObject*> &row, std::vector<WorldObject*> &rowFrom)
{
   for (auto it = rowFrom.begin(); it != rowFrom.end(); ++it)
   {
      WorldObject* objFrom = *it;
      WorldObject* objCopy = objFrom ? objFrom->Clone() : NULL;
      row.push_back(objCopy);
   }
}
void City::CreateRow(std::vector<WorldObject*> &row, int iStart, int iEnd, int housePercent)
{
   int percentBall = 10;
   //for (auto it = row.begin(); it != row.end(); ++it)
   for (int i = iStart; i < iEnd; i++)
   {
      //WorldObject* obj = *it;
      WorldObject* obj = row[i];
      if (obj == NULL)
      {
         //50 percent of houses
         int pHouse = RangedRandInt(0, 100);
         if (pHouse <= housePercent)
         {
            //*it = new House();
            row[i] = new House();
         }
         else 
         {
            int pBall = RangedRandInt(0, 100);
            if (pBall <= percentBall)
            {
               //*it = new Ball();
               row[i] = new Ball();
            }
         }
      }
   }
}

void City::InitCellRow(std::vector<WorldObject*> &row, int cellXPos, double groundHeight, double cellWidth, double cellDepth)
{
#define MAX_FLOOR_NUMBER (14)
   int depth = 0;
   for (auto it = row.begin(); it != row.end(); ++it)
   {
      WorldObject* obj = *it;
      House * house = dynamic_cast<House*>(obj);
      if (house)
      {
         house->GenerateHouse(cellXPos, depth, groundHeight, cellWidth, cellDepth);
      }
      else
      {
         Ball * ball = dynamic_cast<Ball*>(obj);
         if (ball)
         {
            ball->GenerateBall(cellXPos, depth, groundHeight, cellWidth, cellDepth);
         }
      }
      depth += cellDepth;
   }
}


void City::PrepareDraw(World & world)
{
   int i;
   for (i = m_leftRow1.size() - 1; i >= 0; i--)
   {
      world.Add(m_leftRow1[i]);
      world.Add(m_leftRow2[i]);
      world.Add(m_leftRow3[i]);
      world.Add(m_rightRow1[i]);
      world.Add(m_rightRow2[i]);
      world.Add(m_rightRow3[i]);
   }

   for (i = m_balls.size() - 1; i >= 0; i--)
   {
      world.Add(m_balls[i]);
   }
}

#ifndef MOVE_CAMERA
void City::MoveObjects(std::vector<WorldObject*> &row, double dz, double z_houseStep, double z_camera)
{
   for (auto it = row.begin(); it != row.end(); ++it)
   {
      WorldObject* obj = *it;
      if (obj)
      {
         obj->m_pos.z -= dz;
         
         //flip objects backwards to allow endless camera moving
         if (obj->m_pos.z < z_camera - z_houseStep)
         {
            obj->m_pos.z += z_houseStep * m_cellCount;
         }
      }
   }
}
#endif

#ifndef MOVE_CAMERA
void City::MoveObjects(double dz, double z_houseStep, double z_camera)
{
   MoveObjects(m_balls,     dz, z_houseStep, z_camera);
   MoveObjects(m_rightRow1, dz, z_houseStep, z_camera);
   MoveObjects(m_rightRow2, dz, z_houseStep, z_camera);
   MoveObjects(m_rightRow3, dz, z_houseStep, z_camera);
   MoveObjects(m_leftRow1,  dz, z_houseStep, z_camera);
   MoveObjects(m_leftRow2,  dz, z_houseStep, z_camera);
   MoveObjects(m_leftRow3,  dz, z_houseStep, z_camera);
}
#endif

void City::MoveObjects(std::vector<WorldObject*> &row, double dz)
{
   for (auto it = row.begin(); it != row.end(); ++it)
   {
      WorldObject* obj = *it;
      if (obj)
      {
         obj->m_pos.z += dz;
      }
   }
}

void City::MoveObjects(double dz)
{
   MoveObjects(m_balls,     dz);
   MoveObjects(m_rightRow1, dz);
   MoveObjects(m_rightRow2, dz);
   MoveObjects(m_rightRow3, dz);
   MoveObjects(m_leftRow1,  dz);
   MoveObjects(m_leftRow2,  dz);
   MoveObjects(m_leftRow3,  dz);
}

//////////////////////////////////////////////////////////////////////////
BitmapObject::BitmapObject(BitmapObject & other)
: WorldObject(other)
, m_bitmap(other.m_bitmap)
{

}

BitmapObject::BitmapObject(CBitmapObject & m_bitmap)
: m_bitmap(m_bitmap)
{
   BITMAP bm;
   m_bitmap.m_bitmap.GetBitmap(&bm);
   double w = bm.bmWidth;
   double h = bm.bmHeight;
   SetSize(w, h, 0);
}

void BitmapObject::DoDraw(CDC3D & dc)
{
   BITMAP bm;
   m_bitmap.m_bitmap.GetBitmap(&bm);
   double w = bm.bmWidth;
   double h = bm.bmHeight;
   dc.DrawBitmapObject(m_bitmap, m_pos.x, m_pos.y, w, h, m_pos.z);
}


//////////////////////////////////////////////////////////////////////////
Road::Road(double length, double houseStep)
   : m_zStart(-houseStep)
   , m_Length(length)
   , m_LineLength(0.0)
   , m_LineStep(houseStep)
   , m_LineWidth(0.0)
   , m_houseStep(houseStep)
   , m_LineX(0.0)
{

}

Road::~Road()
{

}

void Road::Init(CRect & rc)
{
   m_LineWidth = rc.Width() / 100.0;
   m_LineStep = m_houseStep / 2.0;
   m_LineLength = m_LineStep / 3.0;
   m_LineX = rc.Width() / 2.0;
}

void Road::Draw(CDC3D & dc, RECT* prc)
{
   double groundHeight = ::GetGroundHeight(prc);
   double laneWidth = (prc->right - prc->left) / 10.0;
   double borderWidth = laneWidth / 10.0;
   double borderHeight = borderWidth;
   double roadLeftBorder = laneWidth;
   double roadRightBorder = prc->right - laneWidth;

   //Draw asphalt
#if 1
   {
      //TODO: Fix artifacts, lines drawn from left-upper corner
      COLORREF colAsphalt = RGB(0x28, 0x2B, 0x2A); //Asphalt
      CBrush brushRoad; brushRoad.CreateSolidBrush(colAsphalt);

      CPen     pen;   pen.CreatePen(PS_SOLID, 1, colAsphalt);
      CBrush   brush; brush.CreateSolidBrush(colAsphalt);

      CPen   * oldPen = dc.SelectObject(&pen);
      CBrush * oldBrush = dc.SelectObject(&brush);

      POINT pRoad[3];
      pRoad[0].x = dc.HorizonX();
      pRoad[0].y = dc.HorizonY();

      pRoad[1].x = dc.ProjectionX(prc->right - laneWidth, dc.GetEyeZ());
      pRoad[1].y = dc.ProjectionY(groundHeight, dc.GetEyeZ());

      pRoad[2].x = dc.ProjectionX(roadLeftBorder, dc.GetEyeZ());
      pRoad[2].y = dc.ProjectionY(groundHeight, dc.GetEyeZ());

      ((CDC&)dc).Polygon(pRoad, 3);

      dc.SelectObject(oldPen);
      dc.SelectObject(oldBrush);
   }
#endif

   //Draw central line
   {
      COLORREF colAsphalt = RGB(0xff, 0xff, 0xff); //White line
      CBrush brushRoad; brushRoad.CreateSolidBrush(colAsphalt);

      CPen     pen;   pen.CreatePen(PS_SOLID, 1, colAsphalt);
      CBrush   brush; brush.CreateSolidBrush(colAsphalt);

      CPen   * oldPen = dc.SelectObject(&pen);
      CBrush * oldBrush = dc.SelectObject(&brush);

      POINT3D pLine[4];
      pLine[0].x = m_LineX;
      pLine[0].y = groundHeight;
      pLine[1].x = m_LineX;
      pLine[1].y = groundHeight;
      pLine[2].x = m_LineX + m_LineWidth;
      pLine[2].y = groundHeight;
      pLine[3].x = m_LineX + m_LineWidth;
      pLine[3].y = groundHeight;
      for (double z = m_zStart; z < m_Length; z += m_LineStep)
      {
         pLine[0].z = z;
         pLine[1].z = z + m_LineLength;
         pLine[2].z = z + m_LineLength;
         pLine[3].z = z;

         dc.Polygon(pLine, 4);
      }

      dc.SelectObject(oldPen);
      dc.SelectObject(oldBrush);
   }

   //Draw border stones
   {
      COLORREF colStone        = 0x606060;
      COLORREF colStoneBorder  = 0x202020;

      CPen     pen;   pen.CreatePen(PS_SOLID, 1, colStoneBorder);
      CBrush   brush; brush.CreateSolidBrush(colStone);

      CPen   * oldPen = dc.SelectObject(&pen);
      CBrush * oldBrush = dc.SelectObject(&brush);

      //Left border
      POINT3D pStoneLeftTop[4];
      POINT3D pStoneLeftSide[4];
      pStoneLeftTop[0].x = roadLeftBorder - borderWidth;
      pStoneLeftTop[0].y = groundHeight - borderHeight;
      pStoneLeftTop[1].x = roadLeftBorder - borderWidth;
      pStoneLeftTop[1].y = groundHeight - borderHeight;
      pStoneLeftTop[2].x = roadLeftBorder;
      pStoneLeftTop[2].y = groundHeight - borderHeight;
      pStoneLeftTop[3].x = roadLeftBorder;
      pStoneLeftTop[3].y = groundHeight - borderHeight;

      pStoneLeftSide[0].x = roadLeftBorder;
      pStoneLeftSide[0].y = groundHeight - borderHeight;
      pStoneLeftSide[1].x = roadLeftBorder;
      pStoneLeftSide[1].y = groundHeight - borderHeight;
      pStoneLeftSide[2].x = roadLeftBorder;
      pStoneLeftSide[2].y = groundHeight;
      pStoneLeftSide[3].x = roadLeftBorder;
      pStoneLeftSide[3].y = groundHeight;

      POINT3D pStoneRightTop[4];
      POINT3D pStoneRightSide[4];
      //Right border top
      pStoneRightTop[0].x = roadRightBorder + borderWidth;
      pStoneRightTop[0].y = groundHeight - borderHeight;
      pStoneRightTop[1].x = roadRightBorder + borderWidth;
      pStoneRightTop[1].y = groundHeight - borderHeight;
      pStoneRightTop[2].x = roadRightBorder;
      pStoneRightTop[2].y = groundHeight - borderHeight;
      pStoneRightTop[3].x = roadRightBorder;
      pStoneRightTop[3].y = groundHeight - borderHeight;

      pStoneRightSide[0].x = roadRightBorder;
      pStoneRightSide[0].y = groundHeight - borderHeight;
      pStoneRightSide[1].x = roadRightBorder;
      pStoneRightSide[1].y = groundHeight - borderHeight;
      pStoneRightSide[2].x = roadRightBorder;
      pStoneRightSide[2].y = groundHeight;
      pStoneRightSide[3].x = roadRightBorder;
      pStoneRightSide[3].y = groundHeight;

      for (double z = m_zStart - m_LineStep*2; z < m_Length; z += m_LineLength)
      {
         pStoneLeftTop[0].z = z;
         pStoneLeftTop[1].z = z + m_LineLength;
         pStoneLeftTop[2].z = z + m_LineLength;
         pStoneLeftTop[3].z = z;
         pStoneLeftSide[0].z = z;
         pStoneLeftSide[1].z = z + m_LineLength;
         pStoneLeftSide[2].z = z + m_LineLength;
         pStoneLeftSide[3].z = z;
         dc.Polygon(pStoneLeftTop, 4);
         dc.Polygon(pStoneLeftSide, 4, CDC3D::FaceToRight);

         pStoneRightTop[0].z = z;
         pStoneRightTop[1].z = z + m_LineLength;
         pStoneRightTop[2].z = z + m_LineLength;
         pStoneRightTop[3].z = z;
         pStoneRightSide[0].z = z;
         pStoneRightSide[1].z = z + m_LineLength;
         pStoneRightSide[2].z = z + m_LineLength;
         pStoneRightSide[3].z = z;
         dc.Polygon(pStoneRightTop, 4);
         dc.Polygon(pStoneRightSide, 4, CDC3D::FaceToLeft);
      }

      dc.SelectObject(oldPen);
      dc.SelectObject(oldBrush);
   }
}

void Road::MoveForward(RECT * prc, double dz, double z_camera)
{
   //Move road to the z direction
   m_zStart -= dz;
   if (m_zStart < z_camera - m_houseStep - m_houseStep)
   {
      m_zStart += m_houseStep;
   }
}

//////////////////////////////////////////////////////////////////////////

class BitmapHolder
{
public:
   CBitmapObject m_ballBitmap;
   BitmapHolder()
      : m_ballBitmap(IDB_BALL)
   {
   }
   static BitmapHolder & Instance()
   {
      static BitmapHolder st_instance;
      return st_instance;
   }
};


Ball::Ball() : BitmapObject(BitmapHolder::Instance().m_ballBitmap)
{

}

Ball::Ball(Ball & other) : BitmapObject(other)
{

}

void Ball::GenerateBall(int cellXPos, int cellZPos, double groundHeight, double cellWidth, double cellDepth)
{
   double h = GetSize().h;
   double w = GetSize().w;
   double xShift = RangedRand(0, cellWidth - w);
   double dShift = RangedRand(0, cellDepth - 1);
   SetPos(cellXPos + xShift, groundHeight - h, cellZPos + dShift);
}

WorldObject * Ball::Clone()
{
   return new Ball(*this);
}
