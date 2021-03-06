#ifndef BallInfo_h__
#define BallInfo_h__

#define BALL_MOVE_DELTA (5)
#define MAX_BALLS (10)

typedef struct _BALLINFO 
{
	int width;
   int height;
   int depth;
	int x;
   int y;
   int z;

	int dx;
   int dy;
   int dz;
}BALLINFO;

typedef struct _BALLCOLLISION
{
   BOOL left;
   BOOL right;
   BOOL top;
   BOOL bottom;
   BOOL front;
   BOOL back;
}BALLCOLLISION;

//BALLINFO g_ballInfo;
extern BALLINFO g_balls[];
extern BALLCOLLISION g_ballCollisions[];

void CheckBallCollisions(BALLINFO *ball, BALLCOLLISION *ballCollision, int ballIndex);
void CheckCollisions();
void UpdateBall(BALLINFO *ballInfo, BALLCOLLISION *ballCollision, RECT* screen);
void InitBall(BALLINFO *ballInfo, BITMAP *bm, RECT *screen);



#endif // BallInfo_h__
