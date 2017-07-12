
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
: m_city(HOUSE_COUNT)
, m_road(HOUSE_COUNT * m_z_houseStep, m_z_houseStep)
, m_pressed(key_Nothing)
, m_cxPercent(X_INIT_PERCENT)
, m_cyPercent(Y_INIT_PERCENT)
, m_cameraSpeed(CAMERA_SPEED)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_z_camera = 0.0;
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


   m_city.Init(rc, m_z_houseStep, ::GetGroundHeight(&rc));
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

   //m_z_camera = (m_z_camera + m_cameraSpeed) % m_z_camera_limit;
   //Move camera forever:
   //m_z_camera += m_cameraSpeed;
   //flip camera back for tonnel: if (m_z_camera > m_z_houseStep)
   //flip camera back for tonnel:    m_z_camera = 0.0;
   //m_z_camera_limit;
   RECT rcClient;
   GetClientRect(&rcClient);

   m_city.MoveHouses(&rcClient, m_cameraSpeed, m_city.m_housesLeft.size(), m_z_houseStep, m_z_camera);
   m_road.MoveForward(&rcClient, m_cameraSpeed, m_z_camera);

   Invalidate();
}

void CAnimationWindowStreet::DrawStreet(CDC3D & dc, RECT* prc)
{
   //CBrush brush; brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   //http://www.rapidtables.com/web/color/RGB_Color.htm
   COLORREF colNight = RGB(0, 0, 205);
   COLORREF colDay = RGB(30, 144, 255);// RGB(0, 191, 255);
   COLORREF colGrass = COLOR_GRASSS_BLADE_OF_GRASS;
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
   //m_world.DoDraw(dc, prc);
   m_city.Draw(dc);
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
City::City(int houseCount)
//: m_houseCount(houseCount)
: m_housesLeft(houseCount)  //new House[houseCount])
, m_housesRight(houseCount) //new House[houseCount])
, m_ballBitmap(IDB_BALL)
{
   srand((unsigned)time(NULL));
   for (int i = 0; i < BALL_COUNT; i++)   
   {
      m_balls.push_back(BitmapObject(m_ballBitmap));
   }
}

City::~City()
{
//   delete[] m_housesLeft;
//   delete[] m_housesRight;
}

static double RangedRand(int range_min, int range_max)
{
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   double u = (double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min;
   return u;
}

void City::Init(CRect & rc, double z_houseStep, double groundHeight)
{

   //////////////////////////////////////////////////////////////////////////
   double houseHeight  = House::GetHouseHeight(&rc);
   double houseWidth   = House::GetHouseWidth(&rc);
   double windowHeight = House::GetWindowHeight(&rc);
   double windowWidth  = House::GetWindowWidth(&rc);

   int i;
   //////////////////////////////////////////////////////////////////////////
   // Init random balls
   for (i = 0; i < m_balls.size(); i++)
   {
      m_balls[i].SetPos(RangedRand(0, rc.Width()), groundHeight, RangedRand(0, z_houseStep * m_housesRight.size()));
   }

   //////////////////////////////////////////////////////////////////////////
   int depth = 0;
   for (i = 0; i < m_housesRight.size(); i++)
   {
      //Init right row of houses
      double h = RangedRand(houseHeight / 2, houseHeight * 2);
      double w = RangedRand(houseWidth, houseWidth * 3);
      double d = RangedRand(houseWidth, houseWidth * 2);
      m_housesRight[i].SetPos(rc.right, groundHeight - h, depth);
      m_housesRight[i].SetSize(w, h, d);
      m_housesRight[i].m_windowHeight = windowHeight;
      m_housesRight[i].m_windowWidth = windowWidth;
      depth += z_houseStep;
      
      COLORREF frontCol, sideCol;
      House::GenerateColor(frontCol, sideCol);
      m_housesRight[i].SetColor(frontCol, sideCol);
   }
      
   depth = 0;
   for (i = 0; i < m_housesLeft.size(); i++)
   {
      //Init left row of houses
      double h = RangedRand(houseHeight / 2, houseHeight * 2);
      double w = RangedRand(houseWidth, houseWidth * 3);
      double d = RangedRand(houseWidth, houseWidth * 2);
      m_housesLeft[i].SetPos(0 /*-rc.right / 4*/ - w, groundHeight - h, depth);
      m_housesLeft[i].SetSize(w, h, d);
      m_housesLeft[i].m_windowHeight = windowHeight;
      m_housesLeft[i].m_windowWidth = windowWidth;
      depth += z_houseStep;

      COLORREF frontCol, sideCol;
      House::GenerateColor(frontCol, sideCol);
      m_housesLeft[i].SetColor(frontCol, sideCol);
   }
}

void City::Draw(CDC3D & dc)
{
   World world;
   int i;
   for (i = m_housesLeft.size() - 1; i >= 0; i--)
   {
      world.Add(&m_housesLeft[i]);
      world.Add(&m_housesRight[i]);
      //m_housesLeft[i].DoDraw(dc);
      //m_housesRight[i].DoDraw(dc);
   }

   for (i = m_balls.size() - 1; i >= 0; i--)
   {
      world.Add(&m_balls[i]);
   }

   world.DoDraw(dc);
}

void City::MoveHouses(RECT * prc, double dz, int houseCount, double z_houseStep, double z_camera)
{
   int i;

   for (i = 0; i < m_balls.size(); ++i)
   {
      m_balls[i].m_pos.z -= dz;

      if (m_balls[i].m_pos.z < z_camera - z_houseStep)
      {
         m_balls[i].m_pos.z += z_houseStep*houseCount;
      }
   }

   for (i = 0; i < m_housesRight.size(); i++)
   {
      m_housesRight[i].m_pos.z -= dz;

      //if (m_housesRight[i].m_pos.z < -z_houseStep)
      //{
      //   m_housesRight[i].m_pos.z = z_houseStep*m_housesRight.size();
      //}
      if (m_housesRight[i].m_pos.z < z_camera - z_houseStep)
      {
         m_housesRight[i].m_pos.z += z_houseStep*houseCount;
      }
   }
   
   for (int i = 0; i < m_housesLeft.size(); i++)
   {
      m_housesLeft[i].m_pos.z -= dz;
   
      //if (m_housesLeft[i].m_pos.z < -z_houseStep)
      //{
      //   m_housesLeft[i].m_pos.z = z_houseStep*m_housesLeft.size();
      //}
      if (m_housesLeft[i].m_pos.z < z_camera-z_houseStep)
      {
         m_housesLeft[i].m_pos.z += z_houseStep*houseCount;
      }
   }

}

//////////////////////////////////////////////////////////////////////////
BitmapObject::BitmapObject(CBitmapObject & m_bitmap)
: m_bitmap(m_bitmap)
{

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
   {
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
