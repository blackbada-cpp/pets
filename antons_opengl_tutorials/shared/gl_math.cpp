#include "pch.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "gl_math.h"
#include "string.h"

dp::Vec3::Vec3(const dp::Vec4 & v)
{
   X() = v.X();
   Y() = v.Y();
   Z() = v.Z();
}

dp::Vec3 dp::Vec3::Lerp(const Vec3 & v0, const Vec3 & v1, float t)
{
   Vec3 lerp = v0 + (v1 - v0) * t;
   return lerp;
}

dp::Vec3 dp::Vec3::Lerp(const Vec3 & other, float t)
{
   return Lerp(*this, other, t);
}

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

float dp::Mat4::Determinant() const
{
   const Mat4 & mm = *this;
   return mm.m_data[12] * mm.m_data[9] * mm.m_data[6] * mm.m_data[3] - mm.m_data[8] * mm.m_data[13] * mm.m_data[6] * mm.m_data[3] - mm.m_data[12] * mm.m_data[5] * mm.m_data[10] * mm.m_data[3] + mm.m_data[4] * mm.m_data[13] * mm.m_data[10] * mm.m_data[3] +
      mm.m_data[8] * mm.m_data[5] * mm.m_data[14] * mm.m_data[3] - mm.m_data[4] * mm.m_data[9] * mm.m_data[14] * mm.m_data[3] - mm.m_data[12] * mm.m_data[9] * mm.m_data[2] * mm.m_data[7] + mm.m_data[8] * mm.m_data[13] * mm.m_data[2] * mm.m_data[7] +
      mm.m_data[12] * mm.m_data[1] * mm.m_data[10] * mm.m_data[7] - mm.m_data[0] * mm.m_data[13] * mm.m_data[10] * mm.m_data[7] - mm.m_data[8] * mm.m_data[1] * mm.m_data[14] * mm.m_data[7] + mm.m_data[0] * mm.m_data[9] * mm.m_data[14] * mm.m_data[7] +
      mm.m_data[12] * mm.m_data[5] * mm.m_data[2] * mm.m_data[11] - mm.m_data[4] * mm.m_data[13] * mm.m_data[2] * mm.m_data[11] - mm.m_data[12] * mm.m_data[1] * mm.m_data[6] * mm.m_data[11] + mm.m_data[0] * mm.m_data[13] * mm.m_data[6] * mm.m_data[11] +
      mm.m_data[4] * mm.m_data[1] * mm.m_data[14] * mm.m_data[11] - mm.m_data[0] * mm.m_data[5] * mm.m_data[14] * mm.m_data[11] - mm.m_data[8] * mm.m_data[5] * mm.m_data[2] * mm.m_data[15] + mm.m_data[4] * mm.m_data[9] * mm.m_data[2] * mm.m_data[15] +
      mm.m_data[8] * mm.m_data[1] * mm.m_data[6] * mm.m_data[15] - mm.m_data[0] * mm.m_data[9] * mm.m_data[6] * mm.m_data[15] - mm.m_data[4] * mm.m_data[1] * mm.m_data[10] * mm.m_data[15] + mm.m_data[0] * mm.m_data[5] * mm.m_data[10] * mm.m_data[15];
}

dp::Mat4 dp::Mat4::Inverse() const
{
   Mat4 T;
   const Mat4 & mm = *this;
   float det = mm.Determinant();
   if (0.0f == det) {
      //fprintf(stderr, "WARNING. matrix has no determinant. can not invert\n");
      return mm;
   }
   float inv_det = 1.0f / det;

   float data[ROW_SIZE * COL_SIZE] = {
          inv_det * (mm.m_data[9] * mm.m_data[14] * mm.m_data[7] - mm.m_data[13] * mm.m_data[10] * mm.m_data[7] + mm.m_data[13] * mm.m_data[6] * mm.m_data[11] - mm.m_data[5] * mm.m_data[14] * mm.m_data[11] - mm.m_data[9] * mm.m_data[6] * mm.m_data[15] + mm.m_data[5] * mm.m_data[10] * mm.m_data[15]),
          inv_det * (mm.m_data[13] * mm.m_data[10] * mm.m_data[3] - mm.m_data[9] * mm.m_data[14] * mm.m_data[3] - mm.m_data[13] * mm.m_data[2] * mm.m_data[11] + mm.m_data[1] * mm.m_data[14] * mm.m_data[11] + mm.m_data[9] * mm.m_data[2] * mm.m_data[15] - mm.m_data[1] * mm.m_data[10] * mm.m_data[15]),
          inv_det * (mm.m_data[5] * mm.m_data[14] * mm.m_data[3] - mm.m_data[13] * mm.m_data[6] * mm.m_data[3] + mm.m_data[13] * mm.m_data[2] * mm.m_data[7] - mm.m_data[1] * mm.m_data[14] * mm.m_data[7] - mm.m_data[5] * mm.m_data[2] * mm.m_data[15] + mm.m_data[1] * mm.m_data[6] * mm.m_data[15]),
          inv_det * (mm.m_data[9] * mm.m_data[6] * mm.m_data[3] - mm.m_data[5] * mm.m_data[10] * mm.m_data[3] - mm.m_data[9] * mm.m_data[2] * mm.m_data[7] + mm.m_data[1] * mm.m_data[10] * mm.m_data[7] + mm.m_data[5] * mm.m_data[2] * mm.m_data[11] - mm.m_data[1] * mm.m_data[6] * mm.m_data[11]),
          inv_det * (mm.m_data[12] * mm.m_data[10] * mm.m_data[7] - mm.m_data[8] * mm.m_data[14] * mm.m_data[7] - mm.m_data[12] * mm.m_data[6] * mm.m_data[11] + mm.m_data[4] * mm.m_data[14] * mm.m_data[11] + mm.m_data[8] * mm.m_data[6] * mm.m_data[15] - mm.m_data[4] * mm.m_data[10] * mm.m_data[15]),
          inv_det * (mm.m_data[8] * mm.m_data[14] * mm.m_data[3] - mm.m_data[12] * mm.m_data[10] * mm.m_data[3] + mm.m_data[12] * mm.m_data[2] * mm.m_data[11] - mm.m_data[0] * mm.m_data[14] * mm.m_data[11] - mm.m_data[8] * mm.m_data[2] * mm.m_data[15] + mm.m_data[0] * mm.m_data[10] * mm.m_data[15]),
          inv_det * (mm.m_data[12] * mm.m_data[6] * mm.m_data[3] - mm.m_data[4] * mm.m_data[14] * mm.m_data[3] - mm.m_data[12] * mm.m_data[2] * mm.m_data[7] + mm.m_data[0] * mm.m_data[14] * mm.m_data[7] + mm.m_data[4] * mm.m_data[2] * mm.m_data[15] - mm.m_data[0] * mm.m_data[6] * mm.m_data[15]),
          inv_det * (mm.m_data[4] * mm.m_data[10] * mm.m_data[3] - mm.m_data[8] * mm.m_data[6] * mm.m_data[3] + mm.m_data[8] * mm.m_data[2] * mm.m_data[7] - mm.m_data[0] * mm.m_data[10] * mm.m_data[7] - mm.m_data[4] * mm.m_data[2] * mm.m_data[11] + mm.m_data[0] * mm.m_data[6] * mm.m_data[11]),
          inv_det * (mm.m_data[8] * mm.m_data[13] * mm.m_data[7] - mm.m_data[12] * mm.m_data[9] * mm.m_data[7] + mm.m_data[12] * mm.m_data[5] * mm.m_data[11] - mm.m_data[4] * mm.m_data[13] * mm.m_data[11] - mm.m_data[8] * mm.m_data[5] * mm.m_data[15] + mm.m_data[4] * mm.m_data[9] * mm.m_data[15]),
          inv_det * (mm.m_data[12] * mm.m_data[9] * mm.m_data[3] - mm.m_data[8] * mm.m_data[13] * mm.m_data[3] - mm.m_data[12] * mm.m_data[1] * mm.m_data[11] + mm.m_data[0] * mm.m_data[13] * mm.m_data[11] + mm.m_data[8] * mm.m_data[1] * mm.m_data[15] - mm.m_data[0] * mm.m_data[9] * mm.m_data[15]),
          inv_det * (mm.m_data[4] * mm.m_data[13] * mm.m_data[3] - mm.m_data[12] * mm.m_data[5] * mm.m_data[3] + mm.m_data[12] * mm.m_data[1] * mm.m_data[7] - mm.m_data[0] * mm.m_data[13] * mm.m_data[7] - mm.m_data[4] * mm.m_data[1] * mm.m_data[15] + mm.m_data[0] * mm.m_data[5] * mm.m_data[15]),
          inv_det * (mm.m_data[8] * mm.m_data[5] * mm.m_data[3] - mm.m_data[4] * mm.m_data[9] * mm.m_data[3] - mm.m_data[8] * mm.m_data[1] * mm.m_data[7] + mm.m_data[0] * mm.m_data[9] * mm.m_data[7] + mm.m_data[4] * mm.m_data[1] * mm.m_data[11] - mm.m_data[0] * mm.m_data[5] * mm.m_data[11]),
          inv_det * (mm.m_data[12] * mm.m_data[9] * mm.m_data[6] - mm.m_data[8] * mm.m_data[13] * mm.m_data[6] - mm.m_data[12] * mm.m_data[5] * mm.m_data[10] + mm.m_data[4] * mm.m_data[13] * mm.m_data[10] + mm.m_data[8] * mm.m_data[5] * mm.m_data[14] - mm.m_data[4] * mm.m_data[9] * mm.m_data[14]),
          inv_det * (mm.m_data[8] * mm.m_data[13] * mm.m_data[2] - mm.m_data[12] * mm.m_data[9] * mm.m_data[2] + mm.m_data[12] * mm.m_data[1] * mm.m_data[10] - mm.m_data[0] * mm.m_data[13] * mm.m_data[10] - mm.m_data[8] * mm.m_data[1] * mm.m_data[14] + mm.m_data[0] * mm.m_data[9] * mm.m_data[14]),
          inv_det * (mm.m_data[12] * mm.m_data[5] * mm.m_data[2] - mm.m_data[4] * mm.m_data[13] * mm.m_data[2] - mm.m_data[12] * mm.m_data[1] * mm.m_data[6] + mm.m_data[0] * mm.m_data[13] * mm.m_data[6] + mm.m_data[4] * mm.m_data[1] * mm.m_data[14] - mm.m_data[0] * mm.m_data[5] * mm.m_data[14]),
          inv_det * (mm.m_data[4] * mm.m_data[9] * mm.m_data[2] - mm.m_data[8] * mm.m_data[5] * mm.m_data[2] + mm.m_data[8] * mm.m_data[1] * mm.m_data[6] - mm.m_data[0] * mm.m_data[9] * mm.m_data[6] - mm.m_data[4] * mm.m_data[1] * mm.m_data[10] + mm.m_data[0] * mm.m_data[5] * mm.m_data[10])
   };
   size_t sz = sizeof(data);
   memcpy(T.m_data, data, sz);
   return T;
}

//////////////////////////////////////////////////////////////////////////

#define ONE_DEG_IN_RAD ( 2.0 * M_PI ) / 360.0 // 0.017444444

dp::Quaternion::Quaternion(float degrees, float x, float y, float z)
{
   float rad = ONE_DEG_IN_RAD * degrees;
   m_data[0] = cosf( rad / 2.0f );
   m_data[1] = sinf( rad / 2.0f ) * x;
   m_data[2] = sinf( rad / 2.0f ) * y;
   m_data[3] = sinf( rad / 2.0f ) * z;
}

//dp float dp::Quaternion::Magnitude() const
//dp {
//dp    float m = sqrt(W()*W() + X()*X() + Y()*Y() + Z()*Z());
//dp    return m;
//dp }

void dp::Quaternion::Normalize()
{
   float sum = W()*W() + X()*X() + Y()*Y() + Z()*Z();
   // NB: floats have min 6 digits of precision
   const float thresh = 0.0001f;
   if (fabs(1.0f - sum) < thresh) { return; }
   //dp float mag = Magnitude();
   float mag = sqrt(sum);
   W() = W() / mag;
   X() = X() / mag;
   Y() = Y() / mag;
   Z() = Z() / mag;
}

// convert a unit quaternion to a 4x4 matrix  
//
// q = [w, x, y, z] where w*w + x*x + y*y + z*z = 1
//
// dp ERROR in Anton's book:
//     |1 - 2yy - 2zz  2xy - 2wz      2xz + 2wy      0|
//     |2xy + 2wz      1 - 2xx - 2zz  2yz - 2wx      0|
// M = |2xz - 2wy      2yz + 2wx      1 - 2xx - 2yy  0|
//     |0              0              0              1|
//
// dp Fixed error:
//     |1 - 2yy - 2zz  2xy - 2wz      2xz - 2wy      0|
//     |2xy + 2wz      1 - 2xx - 2zz  2yz + 2wx      0|
// M = |2xz - 2wy      2yz + 2wx      1 - 2xx - 2yy  0|
//     |0              0              0              1|
dp::Mat4 dp::Quaternion::GetMatrix() const
{
   Mat4 M = Mat4::Identity();

   float w = W();
   float x = X();
   float y = Y();
   float z = Z();

#if 0
   //dp error in m_data[8] and m_data[9]
   M.Element(0, 0) = 1.0 - 2.0*y*y - 2.0*z*z;// m_data[0]
   M.Element(0, 1) = 2.0*x*y - 2.0*w*z;      // m_data[4]
   M.Element(0, 2) = 2.0*x*z + 2.0*w*y;      // m_data[8] ERROR

   M.Element(1, 0) = 2.0*x*y + 2.0*w*z;      // m_data[0+1]
   M.Element(1, 1) = 1.0 - 2.0*x*x - 2.0*z*z;// m_data[4+1]
   M.Element(1, 2) = 2.0*y*z - 2.0*w*x;      // m_data[8+1] ERROR
   
   M.Element(2, 0) = 2.0*x*z - 2.0*w*y;      // m_data[0+2]
   M.Element(2, 1) = 2.0*y*z + 2.0*w*x;      // m_data[4+2]
   M.Element(2, 2) = 1 - 2.0*x*x - 2.0*y*y;  // m_data[8+2]
#endif // 0

   //col 0
   M.Element(0, 0) = 1.0f - 2.0f*y*y - 2.0f*z*z;// m_data[0]
   M.Element(1, 0) = 2.0f*x*y + 2.0f*w*z;       // m_data[0+1]
   M.Element(2, 0) = 2.0f*x*z - 2.0f*w*y;       // m_data[0+2]
   M.Element(3, 0) = 0.0f;                      // m_data[0+3]

   //col 1
   M.Element(0, 1) = 2.0f*x*y - 2.0f*w*z;       // m_data[4]
   M.Element(1, 1) = 1.0f - 2.0f*x*x - 2.0f*z*z;// m_data[4+1]
   M.Element(2, 1) = 2.0f*y*z + 2.0f*w*x;       // m_data[4+2]
   M.Element(3, 1) = 0.0f;                      // m_data[4+3]

   //col 2
   M.Element(0, 2) = 2.0f*x*z - 2.0f*w*y;        // m_data[8]  fixed ERROR
   M.Element(1, 2) = 2.0f*y*z + 2.0f*w*x;        // m_data[8+1] fixed ERROR
   M.Element(2, 2) = 1.0f - 2.0f*x*x - 2.0f*y*y; // m_data[8+2]
   M.Element(3, 2) = 0.0f;                       // m_data[8+3]

   //col 3
   M.Element(0, 3) = 0.0f;
   M.Element(1, 3) = 0.0f;
   M.Element(2, 3) = 0.0f;
   M.Element(3, 3) = 1.0f;

   return M;
}

float dp::Quaternion::DotProduct(const Quaternion & r) const
{
   float q0 = Q0();
   float q1 = Q1();
   float q2 = Q2();
   float q3 = Q3();

   float r0 = r.Q0();
   float r1 = r.Q1();
   float r2 = r.Q2();
   float r3 = r.Q3();

   return q0*r0 + q1*r1 + q2*r2 + q3*r3;
}

float dp::Quaternion::DotProduct(const Quaternion & q, const Quaternion & r)
{
   return q.DotProduct(r);
}

dp::Quaternion dp::Quaternion::Slerp(const Quaternion & q, const Quaternion & r, float t)
{
   float dp = q.DotProduct(r);
   if (fabs(dp) >= 1.0f) {
      return q;
   }

   if (dp < 0.0f)
      return Slerp(-q, r, t);

   float sin_omega = sqrt(1.0f - dp * dp);
   Quaternion res;
   if (fabs(sin_omega) < 0.001f) {
      for (int i = 0; i < QUAT_SIZE; i++)
      {
         res.Element(i) = (1.0f - t) * q.Element(i) + t * r.Element(i);
      }
      return res;
   }

   float omega = acos(dp);
   float a = sin((1.0f - t)*omega) / sin_omega;
   float b = sin(t*omega) / sin_omega;
   for (int i = 0; i < QUAT_SIZE; i++)
   {
      res.Element(i) = q.Element(i)*a + r.Element(i)*b;
   }
   return res;
}

dp::Quaternion dp::Quaternion::Slerp(const Quaternion & other, float t) const
{
   return Slerp(*this, other, t);
}

/* multiply quaternions to get another one. result=this*R */
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

   t.Normalize();
   return t;
}
