SOME NOTES FROM CLASS REGARDING MAZE SETUP AND PROCESSING
#define MAZE_BLOCK_COUNT 5
const uint16_t mazeX[]={60,60,60,60,60};
const uint16_t mazeY[]={20,36,52,68,84};

for (i=0;i<MAZE_BLOCK_COUNT;i++)
		putImage(mazeX[i],mazeY[i],12,16,dg1,0,0);


int hitMaze(uint16_t playerx,uint16_t playery,uint16_t w, uint16_t h)
{
	int rvalue = 0;
	
	int mazeblock=0;
	for (mazeblock = 0; mazeblock < MAZE_BLOCK_COUNT; mazeblock++)
	{
		if (isInside(mazeX[mazeblock],mazeY[mazeblock],w,h,playerx,playery)==1)
		{
			rvalue = 1;
			break;
		}
		if (isInside(mazeX[mazeblock],mazeY[mazeblock],w,h,playerx+12,playery)==1)
		{
			rvalue = 1;
			break;
		}
		if (isInside(mazeX[mazeblock],mazeY[mazeblock],w,h,playerx+12,playery+16)==1)
		{
			rvalue = 1;
			break;
		}
		if (isInside(mazeX[mazeblock],mazeY[mazeblock],w,h,playerx,playery+16)==1)
		{
			rvalue = 1;
			break;
		}

	}
	return rvalue;
}
