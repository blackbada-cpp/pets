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

dp::Mat4 dp::Mat4::LookYaw(Vec3 cameraPosition, float camera_yaw)
{
   Mat4 T = Translation(-cameraPosition);
   Mat4 R = RotationY(camera_yaw);
   Mat4 viewMatrix = R * T;
   return viewMatrix;
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

//////////////////////////////////////////////////////////////////////////

dp::Quaternion::Quaternion(float angle, float x, float y, float z)
{
   m_data[0] = cos(angle/2.0);
   m_data[1] = sin(angle/2.0) * x;
   m_data[2] = sin(angle/2.0) * y;
   m_data[3] = sin(angle/2.0) * z;
}

float dp::Quaternion::Magnitude() const
{
   float m = sqrt(W()*W() + X()*X() + Y()*Y() + Z()*Z());
   return m;
}

void dp::Quaternion::Normalize()
{
   float m = Magnitude();
   W() = W() / m;
   X() = X() / m;
   Y() = Y() / m;
   Z() = Z() / m;
}

dp::Mat4 dp::Quaternion::GetMatrix() const
{
   Mat4 M = Mat4::Identity();

   float w = W();
   float x = X();
   float y = Y();
   float z = Z();

   M.Element(0, 0) = 1.0 - 2.0*y*y - 2.0*z*z;
   M.Element(0, 1) = 2.0*x*y - 2.0*w*z;
   M.Element(0, 2) = 2.0*x*z + 2.0*w*y;

   M.Element(1, 0) = 2.0*x*y + 2.0*w*z;
   M.Element(1, 1) = 1.0 - 2.0*x*x - 2.0*z*z;
   M.Element(1, 2) = 2.0*y*z - 2.0*w*x;
   
   M.Element(2, 0) = 2.0*x*z - 2.0*w*y;
   M.Element(2, 1) = 2.0*y*z + 2.0*w*x;
   M.Element(2, 2) = 1 - 2.0*x*x - 2.0*y*y;

   return M;
}

dp::Quaternion dp::Quaternion::operator*(const Quaternion & r) const
{
   float q0 = Q0();
   float q1 = Q1();
   float q2 = Q2();
   float q3 = Q3();
   
   float r0 = r.Q0();
   float r1 = r.Q1();
   float r2 = r.Q2();
   float r3 = r.Q3();

   Quaternion t;
   t.Q0() = r0*q0 - r1*q1 - r2*q2 - r3*q3;
   t.Q1() = r0*q1 + r1*q0 - r2*q3 + r3*q2;
   t.Q2() = r0*q2 + r1*q3 + r2*q0 - r3*q1;
   t.Q3() = r0*q3 - r1*q2 + r2*q1 + r3*q0;
   return t;
}
