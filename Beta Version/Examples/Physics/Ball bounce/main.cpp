#include "winsimple.hpp"


void getNormal(ws::Vec2i v1,ws::Vec2i v2)
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	
	float nx = -dy;//swap and nagate so that vector is rotated 90 degrees.
	float ny = dx;
	
	
	
}

int main()
{
	ws::Line walls[4];

	
	ws::Window window(960,540,"");
	int offset = 20;
	int direction = 1;
	while(window.isOpen())
	{
		if(offset > 540)
			direction = -1;
		if(offset < 0)
			direction = 1;
		offset += direction;

		walls[0].start = {0,offset};
		walls[0].end = {960-offset,0};
		walls[1].start = walls[0].end;
		walls[1].end = {960,540-offset};
		walls[2].start = walls[1].end;
		walls[2].end = {offset,540};
		walls[3].start = walls[2].end;
		walls[3].end = walls[0].start;

		float normal = getNormal(walls[0].start,walls[0].end);

		window.clear();
		for(int a=0;a<4;a++)
		{
			window.draw(walls[a]);
		}
		window.display();
	}
	return 0;
}