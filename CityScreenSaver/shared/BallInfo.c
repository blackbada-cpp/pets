#include <crtdbg.h>
#include <windows.h>
#include "BallInfo.h"

BALLINFO g_balls[MAX_BALLS];
BALLCOLLISION g_ballCollisions[MAX_BALLS];

void CheckBallCollisions(BALLINFO *ball, BALLCOLLISION *ballCollision, int ballIndex)
{
#define COLLISION_ALLOWANCE  (2)
#define COLLISION_ALLOWANCE_DOUBLE (4)
   BALLINFO *otherBall;
   int j;
   int dx, dy, dz;
   int x1,x2,y1,y2,z1,z2,w,h,d;
   for (j=0; j<MAX_BALLS; j++)
   {
      if (j==ballIndex)
         continue;
      otherBall = &g_balls[j];
      
      dx = otherBall->x - ball->x;
      dy = otherBall->y - ball->y;
      dz = otherBall->z - ball->z;

      //Skip if balls don't touch each other
      if(abs(dx) > ball->width  - COLLISION_ALLOWANCE_DOUBLE || 
         abs(dy) > ball->height - COLLISION_ALLOWANCE_DOUBLE ||
         abs(dz) > ball->depth  - COLLISION_ALLOWANCE_DOUBLE)
         continue;
      
      x1 = ball->x;
      x2 = otherBall->x;
      w = ball->width;
      y1 = ball->y;
      y2 = otherBall->y;
      h = ball->height;
      z1 = ball->z;
      z2 = otherBall->z;
      d = ball->depth;

      //Сollision from the right
      if (!ballCollision->right)
      {
         if(x1<x2 && x1+w>x2)
         {
            ballCollision->right = TRUE;
         }
      }
      
      //Left collision
      if (!ballCollision->left)
      {
         if(x1>x2 && x1<x2+w)
         {
            ballCollision->left = TRUE;
         }
      }

      //Only for 2 balls
      //_ASSERT(!ballCollision->left || !ballCollision->right);

      //Top collision
      if(!ballCollision->top)
      {
         if(y1>y2 && y1<y2+h)
         {
            ballCollision->top = TRUE;
         }
      }

      //Bottom collision
      if (!ballCollision->bottom)
      {
         if(y1<y2 && y1+h>y2)
         {
            ballCollision->bottom = TRUE;
         }
      }

      //Only for 2 balls
      //_ASSERT(!ballCollision->top || !ballCollision->bottom);

      //Front collision
      if (!ballCollision->front)
      {
         if (z1 > z2 && z1 < z2 + d)
         {
            ballCollision->front = TRUE;
         }
      }

      //Back collision
      if (!ballCollision->back)
      {
         if (z1 < z2 && z1 + d > z2)
         {
            ballCollision->back = TRUE;
         }
      }
   }
}

void CheckCollisions()
{
   int i;
   char buf[1024];
   ZeroMemory(g_ballCollisions, sizeof(g_ballCollisions));

   for(i=0; i<MAX_BALLS; i++)
   {
      CheckBallCollisions(&g_balls[i], &g_ballCollisions[i], i);
   }

   for(i=0; i<MAX_BALLS; i++)
   {
      BALLCOLLISION * col = &g_ballCollisions[i];
      sprintf(buf, "COL#%d: %s %s %s %s\n", i, 
         col->left  ? "<-" : "",
         col->right ? "->" : "",
         col->top   ? "T"  : "",
         col->bottom? "B"  : "");
      _RPT0(_CRT_WARN, buf);
   }
}

const ScreenDepth(RECT *screen) {
   int dx = screen->right - screen->left;
   int dy = screen->bottom - screen->top;
   int dz = max(dx, dy) * 7;
   return dz;
}

void UpdateBall(BALLINFO *ballInfo, BALLCOLLISION *ballCollision, RECT* screen)
{
	ballInfo->x += ballInfo->dx;
   ballInfo->y += ballInfo->dy;
   ballInfo->z += ballInfo->dz;

   //Left and right collision
   if(ballCollision->left)
      ballInfo->dx = BALL_MOVE_DELTA;
   else if(ballCollision->right)
      ballInfo->dx = -BALL_MOVE_DELTA;

   //Top and bottom collision
   if(ballCollision->top)
      ballInfo->dy = BALL_MOVE_DELTA;
   else if(ballCollision->bottom)
      ballInfo->dy = -BALL_MOVE_DELTA;

   //front and back collizion
   if (ballCollision->front)
      ballInfo->dz = BALL_MOVE_DELTA;
   else if (ballCollision->back)
      ballInfo->dz = -BALL_MOVE_DELTA;

   //Screen left and right limits
	if(ballInfo->x < 0)
	{
		ballInfo->x = 0;
		ballInfo->dx = BALL_MOVE_DELTA;
	}
	else if(ballInfo->x + ballInfo->width > screen->right)
	{
		ballInfo->x = screen->right - ballInfo->width;
		ballInfo->dx = -BALL_MOVE_DELTA;
	}

   //Screen top and bottom limits
   if(ballInfo->y < 0)
	{
		ballInfo->y = 0;
		ballInfo->dy = BALL_MOVE_DELTA;
	}
	else if(ballInfo->y + ballInfo->height > screen->bottom)
	{
		ballInfo->y = screen->bottom - ballInfo->height;
		ballInfo->dy = -BALL_MOVE_DELTA;
	}

   //Screen front and back limits
   if (ballInfo->z < 0)
   {
      ballInfo->z = 0;
      ballInfo->dz = BALL_MOVE_DELTA;
   }
   else if (ballInfo->z + ballInfo->depth > ScreenDepth(screen))
   {
      ballInfo->z = ScreenDepth(screen) - ballInfo->depth;
      ballInfo->dz = -BALL_MOVE_DELTA;
   }
}


void InitBall(BALLINFO *ballInfo, BITMAP *bm, RECT *screen)
{
   double x, y, z, stretch;
   int dx, dy, dz;
   ZeroMemory(ballInfo, sizeof(*ballInfo));

   //Calculate ball size x0.1 ... x10
   stretch = 2.0;// (double)rand() / RAND_MAX * 9.0 + 1.0;
   //if ((rand() > RAND_MAX / 2))
   {
      ballInfo->width = stretch*bm->bmWidth;
      ballInfo->height = stretch*bm->bmHeight;
      ballInfo->depth = stretch*bm->bmHeight;
   }
   //else
   //{
   //   ballInfo->width = (double)bm->bmWidth / stretch;
   //   ballInfo->height = (double)bm->bmHeight / stretch;
   //   ballInfo->depth = (double)bm->bmHeight / stretch;
   //}

   //Calculate ball coords
   x = (double)rand();
   dx = screen->right - screen->left - ballInfo->width;
   ballInfo->x = x/RAND_MAX*dx;
   
   y =(double)rand();
   dy = screen->bottom - screen->top - ballInfo->height;
   ballInfo->y = y/RAND_MAX*dy;

   z = (double)rand();
   dz = ScreenDepth(screen) - ballInfo->depth;
   ballInfo->z = z / RAND_MAX*dz;

   //Calculate ball direction
   ballInfo->dx = BALL_MOVE_DELTA * ((rand() > RAND_MAX / 2) ? 1 : -1);
   ballInfo->dy = BALL_MOVE_DELTA * ((rand() > RAND_MAX / 2) ? 1 : -1);
   ballInfo->dz = BALL_MOVE_DELTA * ((rand() > RAND_MAX / 2) ? 1 : -1);
}

