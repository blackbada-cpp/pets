#include "pch.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "gl_math.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

float dp::Vec3::Magnitude() const
{
   float m = sqrt(X()*X() + Y()*Y() + Z()*Z());
   return m;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//     | 1   0       0    0 |
// Rx= | 0 cos(a) -sin(a) 0 |
//     | 0 sin(a)  cos(a) 0 |
//     | 0   0       0    1 |
void dp::Mat4::SetRotationX(float angle)
{
   Element(1, 1) = cos(angle);
   Element(1, 2) = -sin(angle);
   Element(2, 1) = sin(angle);
   Element(2, 2) = cos(angle);
}

//     | cos(a) 0 sin(a) 0 |
// Ry= |   0    1   0    0 |
//     |-sin(a) 0 cos(a) 0 |
//     |   0    0   0    1 |
void dp::Mat4::SetRotationY(float angle)
{
   Element(0, 0) = cos(angle);
   Element(0, 2) = sin(angle);
   Element(2, 0) = -sin(angle);
   Element(2, 2) = cos(angle);
}

//     |cos(a) -sin(a) 0 0 |
// Rz= |sin(a)  cos(a) 0 0 |
//     |  0       0    1 0 |
//     |  0       0    0 1 |
void dp::Mat4::SetRotationZ(float angle)
{
   Element(0, 0) = cos(angle);
   Element(0, 1) = -sin(angle);
   Element(1, 0) = sin(angle);
   Element(1, 1) = cos(angle);
}

dp::Mat4 dp::Mat4::ViewRotation(Vec3 upward, Vec3 forward, Vec3 right)
{
   Mat4 R;
   R = Identity();
   R.SetViewUpwardVector(upward);
   R.SetViewForwardVector(-forward);
   R.SetViewRightVector(right);
   return R;
}

// Column-major View Matrix
// |  Rx  Ry  Rx -Px |
// |  Ux  Uy  Uz -Py |
// | -Fx -Fy -Fz -Pz |
// |  0   0   0   1  |
// Where U - is a unit vector pointing up-wards, 
//       F - unit vector pointing forwards, 
//       R - unit vector pointing right, 
//   and P - is the position of the camera, all in world coordinates. 
// The U vector must change as the camera pitches forwards and back, or rolls to either side.
dp::Mat4 dp::Mat4::View(Vec3 upward, Vec3 forward, Vec3 right, Vec3 cameraPosition)
{
   Mat4 R = ViewRotation(upward, forward, right);
   Mat4 T = Translation(-cameraPosition);

   //dp Mat4 viewMatrixOld;
   //dp viewMatrixOld = Identity();
   //dp viewMatrixOld.SetViewUpwardVector(upward);
   //dp viewMatrixOld.SetViewForwardVector(-forward);
   //dp viewMatrixOld.SetViewRightVector(right);
   //dp viewMatrixOld.SetViewPosition(-cameraPosition);

   Mat4 viewMatrix = R * T; //dp equals viewMatrixOld
   return viewMatrix;
}

dp::Mat4 dp::Mat4::LookAt(Vec3 cameraPosition, Vec3 targetPosition, Vec3 upwardDirection)
{
   //dp Mat4 T = Translation(-cameraPosition);
   Vec3 d = targetPosition - cameraPosition;
   float mag = d.Magnitude();
   Vec3 f = d / mag; //forward looking normalized vector
   Vec3 r = f.CrossProduct(upwardDirection); // right vector r = f x u
   Vec3 u = r.CrossProduct(f); // u = r x f

   //dp Mat4 R = ViewRotation(u, f, r);
   //dp Mat4 V = R * T;
   //dp return V;
   return View(u, f, r, cameraPosition);
}

#define GRAD2RAD  (M_PI/180.0)
#define RAD2GRAD  (180.0/M_PI)
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.01744444

dp::Mat4 dp::Mat4::Projection(float screenWidth, float screenHeight)
{
   Mat4 Proj;
   
   float nearPlane = 0.1f;
   float farPlane = 100.0f;
   float fov = 67.0 * ONE_DEG_IN_RAD; // convert 67 degrees to radians
   float aspect = screenWidth / screenHeight;
   //matrix components
   float range = tan(fov*0.5f) * nearPlane;

   float Sx = (2.0f * nearPlane) / (range*aspect + range*aspect);
   float Sy = nearPlane/range;
   float Sz = -(farPlane + nearPlane) / (farPlane - nearPlane);
   float Pz = -(2.0*farPlane*nearPlane) / (farPlane - nearPlane);
   Proj.Element(0, 0) = Sx;
   Proj.Element(1, 1) = Sy;
   Proj.Element(2, 2) = Sz;
   Proj.Element(2, 3) = Pz;
   Proj.Element(3, 2) = -1.0f;
   return Proj;
}

