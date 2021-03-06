#pragma once
#include <assert.h>

namespace dp
{
#define VEC3_SIZE (3)
#define ROW_SIZE (4)
#define COL_SIZE (4)
#define QUAT_SIZE (4)
   class Vec4;

   // �olumn-major vector with 3 items
   // | #0 |
   // | #1 |
   // | #2 |
   class Vec3
   {
   public:
      float m_data[VEC3_SIZE];
      Vec3()
      {
         Clear();
      }
      Vec3(float x, float y, float z)
      {
         m_data[0] = x;
         m_data[1] = y;
         m_data[2] = z;
      }
      Vec3(const dp::Vec4 & v);
      inline float & X() { return m_data[0]; }
      inline float & Y() { return m_data[1]; }
      inline float & Z() { return m_data[2]; }
      inline const float & X() const { return m_data[0]; }
      inline const float & Y() const { return m_data[1]; }
      inline const float & Z() const { return m_data[2]; }
      Vec3(const Vec3 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
      }
      Vec3 & operator = (const Vec3 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];

         return *this;
      }
      
      //Add vectors
      Vec3 operator+ (const Vec3 & other) const
      {
         Vec3 res;
         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) + other.Element(i);

         return res;
      }
      
      //Subtract vectors
      Vec3 operator- (const Vec3 & other) const
      {
         Vec3 res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) - other.Element(i);

         return res;
      }

      Vec3 operator-() const
      {
         Vec3 res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = -Element(i);

         return res;
      }

      //Multiply by constant
      Vec3 operator* (float c) const
      {
         Vec3 res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) * c;

         return res;
      }
      
      //Divide by constant
      Vec3 operator/ (float c) const
      {
         Vec3 res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) / c;

         return res;
      }

      // aka vector multiplication 
      // https://en.wikipedia.org/wiki/Cross_product
      // | a1 |   | b1 |   | a2*b3 - a3*b2 |
      // | a2 | x | b2 | = | a3*b1 - a1*b3 |
      // | a3 |   | b3 |   | a1*b2 - a2*b1 |
      Vec3 CrossProduct(const Vec3 & b) const
      {
         float a1 = X(), a2 = Y(), a3 = Z();
         float b1 = b.X(), b2 = b.Y(), b3 = b.Z();
         return Vec3(a2*b3 - a3 * b2, 
                     a3*b1 - a1 * b3, 
                     a1*b2 - a2 * b1);
      }
      
      // aka scalar multiplication 
      // https://en.wikipedia.org/wiki/Dot_product
      float DotProduct(const Vec3 & b) const
      {
         float a1 = X(), a2 = Y(), a3 = Z();
         float b1 = b.X(), b2 = b.Y(), b3 = b.Z();
         return (a1*b1 + a2*b2 + a3*b3);
      }

      // Interpolation
      static Vec3 Lerp(const Vec3 & v0, const Vec3 & v1, float t);
      Vec3 Lerp(const Vec3 & other, float t);

      float Magnitude() const;
      float Length() const { return Magnitude(); }
      Vec3 Normalize() const;
      inline int Count() const { return VEC3_SIZE; }
      const float & Element(int row) const
      {
         // row = 0..2
         assert(row >= 0 && row < VEC3_SIZE);
         return m_data[row];
      }
      float & Element(int row)
      {
         assert(row >= 0 && row < VEC3_SIZE);
         return m_data[row];
      }
      void Clear()
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = 0.0;
      }

   }; //Vec3
   
   // �olumn-major vector with 4 items, represented as one vertical column
   // | #0 |
   // | #1 |
   // | #2 |
   // | #3 |
   class Vec4
   {
   public:
      float m_data[COL_SIZE];
      Vec4()
      {
         Clear();
      }
      Vec4(float x, float y, float z, float w)
      {
         m_data[0] = x;
         m_data[1] = y;
         m_data[2] = z;
         m_data[3] = w;
      }
      inline float & X() { return m_data[0]; }
      inline float & Y() { return m_data[1]; }
      inline float & Z() { return m_data[2]; }
      inline float & W() { return m_data[3]; }
      inline const float & X() const { return m_data[0]; }
      inline const float & Y() const { return m_data[1]; }
      inline const float & Z() const { return m_data[2]; }
      inline const float & W() const { return m_data[3]; }

      Vec4(const Vec4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
      }
      Vec4 & operator = (const Vec4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];

         return *this;
      }
      inline int Count() const { return COL_SIZE; }
      const float & Element(int row) const
      {
         // row = 0..3
         assert(row >= 0 && row < COL_SIZE);
         return m_data[row];
      }
      float & Element(int row) 
      {
         assert(row >= 0 && row < COL_SIZE);
         return m_data[row];
      }
      void Clear()
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = 0.0;
      }
   }; //Vec4


   // Column-major matrix with 16 items indexed this way:
   // | #0 #4 #8   #12 |
   // | #1 #5 #9   #13 |
   // | #2 #6 #10  #14 |
   // | #3 #7 #11  #15 |
   //
   // Notes: column-major matrix seems transposed, but it's a way we iterate arrays:
   // float m_data[] = {
   //    1.0f, 0.0f, 0.0f, 0.0f, //1st column {#0, #1, #2, #3,
   //    0.0f, 1.0f, 0.0f, 0.0f, //2nd column  #4, #5, #6, #7,
   //    0.0f, 0.0f, 1.0f, 0.0f, //3rd column  #8, #9, #10, #11,
   //    0.5f, 0.0f, 0.0f, 1.0f  //4th column  #12, #13, #14, #15}
   // };

   class Mat4
   {
   public:
      float m_data[ROW_SIZE * COL_SIZE];

   public:
      Mat4() 
      {
         Clear();
      }
      Mat4(const Mat4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
      }
      Mat4 & operator = (const Mat4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
         
         return *this;
      }
      operator float*() 
      {
         return &m_data[0];
      }
      operator const float*() const
      {
         return &m_data[0];
      }


      //////////////////////////////////////////////////////////////////////////
   public:

      void Clear()
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = 0;
      }

      inline int Count() const { return ROW_SIZE * COL_SIZE; }
      float & Element(int row, int col)
      {
         // row = 0..3, col=0..3
         assert(row >= 0 && row < COL_SIZE);
         assert(col >= 0 && col < ROW_SIZE);
         return m_data[col*COL_SIZE + row];
      }
      const float & Element(int row, int col) const 
      {
         // row = 0..3, col=0..3
         assert(row >= 0 && row < COL_SIZE);
         assert(col >= 0 && col < ROW_SIZE);
         return m_data[col*COL_SIZE + row];
      }

      // Matrix * Vector in column-major form
      // | #0 #4 #8   #12 |
      // | #1 #5 #9   #13 |
      // | #2 #6 #10  #14 |
      // | #3 #7 #11  #15 |
      // | a b c d |  | x |    |ax + by + cz + dw|
      // | e f g h |  | y |  = |ex + fy + gz + hw|
      // | i j k l |  | z |    |ix + jy + kz + lw|
      // | m n o p |  | w |    |mx + ny + oz + pw|
      Vec4 operator *(const Vec4 & other) const
      {
         Vec4 res;

         for (int irow = 0; irow < COL_SIZE; irow++)
         {
            res.Element(irow) = 0.0;
            for (int icol = 0; icol < ROW_SIZE; icol++)
            {
               res.Element(irow) += Element(irow, icol) * other.Element(icol);
            }
         }
         return res; //copy
      }

      Mat4 operator *(const Mat4 & other) const
      {
         Mat4 res;

         for (int irow = 0; irow < COL_SIZE; irow++) //i
         {
            for (int icol = 0; icol < ROW_SIZE; icol++) //j
            {
               res.Element(irow, icol) = 0.0;
               for (int i = 0; i < ROW_SIZE; i++) //1,2,3,4
               {
                  //from https://en.wikipedia.org/wiki/Matrix_multiplication
                  // Cij = Ai1*B1j + Ai2*B2j + Ai3*B3j + Ai4*B4j
                  res.Element(irow, icol) += Element(irow, i) * other.Element(i, icol);
               }
            }
         }
         return res; //copy
      }

   private:
      static Mat4 GetIdentity()
      {
         Mat4 res;
         for (int i = 0; i < ROW_SIZE; i++)
            res.Element(i, i) = 1.0;
         return res;
      }

   public:
      static const Mat4 & Identity()
      {
         static Mat4 res = GetIdentity();
         return res;
      }

      float Determinant() const;

      // return inverse Matrix M-1 https://de.wikipedia.org/wiki/Inverse_Matrix
      // Note: MM-1 = E
      Mat4 Inverse() const;

      //////////////////////////////////////////////////////////////////////////
      // Translation column-major matrix:
      // | 1 0 0 Tx |
      // | 0 1 0 Ty |
      // | 0 0 1 Tz |
      // | 0 0 0 1  |
      void SetTranslation(float x, float y, float z)
      {
         TranslationX() = x;
         TranslationY() = y;
         TranslationZ() = z;
      }
      inline float & TranslationX() { return Element(0, 3); }
      inline float & TranslationY() { return Element(1, 3); }
      inline float & TranslationZ() { return Element(2, 3); }
      inline const float & TranslationX() const { return Element(0, 3); }
      inline const float & TranslationY() const { return Element(1, 3); }
      inline const float & TranslationZ() const { return Element(2, 3); }
      static Mat4 Translation(float x, float y, float z)
      {
         Mat4 T;
         T = Identity();
         T.SetTranslation(x, y, z);
         return T;
      }
      static Mat4 Translation(Vec3 T)
      {
         return Translation(T.X(), T.Y(), T.Z());
      }

      //////////////////////////////////////////////////////////////////////////
      void SetRotationX(float angle);
      void SetRotationY(float angle);
      void SetRotationZ(float angle);
      static Mat4 RotationX(float angle)
      {
         Mat4 Rx;
         Rx = Identity();
         Rx.SetRotationX(angle);
         return Rx;
      }
      static Mat4 RotationY(float angle)
      {
         Mat4 Ry;
         Ry = Identity();
         Ry.SetRotationY(angle);
         return Ry;
      }
      static Mat4 RotationZ(float angle)
      {
         Mat4 Rz;
         Rz = Identity();
         Rz.SetRotationZ(angle);
         return Rz;
      }

      //////////////////////////////////////////////////////////////////////////
      // Scale column-major matrix:
      // | Sx 0  0  0 |
      // | 0  Sy 0  0 |
      // | 0  0  Sz 0 |
      // | 0  0  0  1 |
      void SetScale(float x, float y, float z)
      {
         ScaleX() = x;
         ScaleY() = y;
         ScaleZ() = z;
      }
      inline float & ScaleX() { return Element(0, 0); }
      inline float & ScaleY() { return Element(1, 1); }
      inline float & ScaleZ() { return Element(2, 2); }
      inline const float & ScaleX() const { return Element(0, 0); }
      inline const float & ScaleY() const { return Element(1, 1); }
      inline const float & ScaleZ() const { return Element(2, 2); }
      static Mat4 Scale(float x, float y, float z)
      {
         Mat4 S;
         S = Identity();
         S.SetScale(x, y, z);
         return S; //copy
      }

      //////////////////////////////////////////////////////////////////////////
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
      static Mat4 View(Vec3 Upward, Vec3 Forward, Vec3 Right, Vec3 CameraPosition);
      static Mat4 ViewRotation(Vec3 Upward, Vec3 Forward, Vec3 Right);

      // Get View matrix from camera position and up vector
      static Mat4 LookAt(Vec3 camera_position, Vec3 target_position, Vec3 up_direction);

      //GetView matrix from camera position and camera rotation
      static Mat4 LookYaw(Vec3 cameraPosition, float camera_yaw);

      void SetViewUpwardVector(Vec3 Upward) { SetViewUpwardVector(Upward.X(), Upward.Y(), Upward.Z()); }
      void SetViewUpwardVector(float x, float y, float z)
      {
         Element(1, 0) = x;
         Element(1, 0) = y;
         Element(1, 0) = z;
      }
      inline void SetViewForwardVector(Vec3 Forward) { SetViewForwardVector(Forward.X(), Forward.Y(), Forward.Z()); }
      void SetViewForwardVector(float x, float y, float z)
      {
         Element(2, 0) = x;
         Element(2, 1) = y;
         Element(2, 2) = z;
      }
      inline void SetViewRightVector(Vec3 Right) { SetViewRightVector(Right.X(), Right.Y(), Right.Z()); }
      void SetViewRightVector(float x, float y, float z)
      {
         Element(0, 0) = x;
         Element(0, 1) = y;
         Element(0, 2) = z;
      }
      inline void SetViewPosition(Vec3 Pos) { SetViewPosition(Pos.X(), Pos.Y(), Pos.Z()); }
      void SetViewPosition(float x, float y, float z)
      {
         Element(0, 3) = x;
         Element(1, 3) = y;
         Element(2, 3) = z;
      }

      //////////////////////////////////////////////////////////////////////////
      //Column-major projection matrix
      // | Sx 0  0  0  |
      // | 0  Sy 0  0  |
      // | 0  0  Sz Pz |
      // | 0  0  -1 0  |
      static Mat4 Projection(float screenWidth, float screenHeight);
   }; //Mat4

   class Quaternion
   {
   public:
      float m_data[QUAT_SIZE];
      Quaternion()
      {
         Clear();
      }
      Quaternion(float angle, float x, float y, float z);
      inline float & Q0() { return m_data[0]; }
      inline float & Q1() { return m_data[1]; }
      inline float & Q2() { return m_data[2]; }
      inline float & Q3() { return m_data[3]; }
      inline const float & Q0() const { return m_data[0]; }
      inline const float & Q1() const { return m_data[1]; }
      inline const float & Q2() const { return m_data[2]; }
      inline const float & Q3() const { return m_data[3]; }

      inline float & W() { return Q0(); }
      inline float & X() { return Q1(); }
      inline float & Y() { return Q2(); }
      inline float & Z() { return Q3(); }
      inline const float & W() const { return Q0(); }
      inline const float & X() const { return Q1(); }
      inline const float & Y() const { return Q2(); }
      inline const float & Z() const { return Q3(); }

      Quaternion(const Quaternion & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];
      }
      Quaternion & operator = (const Vec4 & other)
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = other.m_data[i];

         return *this;
      }

      inline int Count() const { return QUAT_SIZE; }
      const float & Element(int row) const
      {
         // row = 0..3
         assert(row >= 0 && row < QUAT_SIZE);
         return m_data[row];
      }
      float & Element(int row)
      {
         assert(row >= 0 && row < QUAT_SIZE);
         return m_data[row];
      }
      void Clear()
      {
         for (int i = 0; i < Count(); i++)
            m_data[i] = 0.0;
      }

      //Add vectors
      Quaternion operator+ (const Quaternion & other) const
      {
         Quaternion res;
         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) + other.Element(i);

         return res;
      }

      //Subtract vectors
      Quaternion operator- (const Quaternion & other) const
      {
         Quaternion res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) - other.Element(i);

         return res;
      }

      Quaternion operator-() const
      {
         Quaternion res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = -Element(i);

         return res;
      }

      //Multiply by constant
      Quaternion operator* (float c) const
      {
         Quaternion res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) * c;

         return res;
      }

      //Divide by constant
      Quaternion operator/ (float c) const
      {
         Quaternion res;

         for (int i = 0; i < Count(); i++)
            res.Element(i) = Element(i) / c;

         return res;
      }

      //dp float Magnitude() const;
      void Normalize();
      Mat4 GetMatrix() const;

      Quaternion operator *(const Quaternion & other) const;
      
      static float DotProduct(const Quaternion & q, const Quaternion & r);
      float DotProduct(const Quaternion & other) const;
      
      static Quaternion Slerp(const Quaternion & q, const Quaternion & r, float t);
      Quaternion Slerp(const Quaternion & other, float t) const;
   };
} //dp