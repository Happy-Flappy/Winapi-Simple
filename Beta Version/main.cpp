#include <iostream>
#include "winsimple.h"




	
	
const int gridWidth = 960/32;
const int gridHeight = 17;

bool win = false;


void drawLevel(int grid[gridHeight][gridWidth],ws::Window &window,ws::Sprite &spr,ws::Radial &circle)
{
	
	
	
	for(int x=0;x<gridWidth;x++)
	{
		for(int y=0;y<gridHeight;y++)
		{
			

			
			if(grid[y][x] == 1)
			{
				spr.setTextureRect({0,0,32,32});
				spr.x = x*32;
				spr.y = y*32;
				window.draw(spr);
			}
			if(grid[y][x] == 2)
			{
				spr.setTextureRect({32,0,32,32});
				spr.x = x*32;
				spr.y = y*32;
				window.draw(spr);
				
			}
			if(grid[y][x] == 3)
			{
				spr.setTextureRect({32*2,0,32,32});
				spr.x = x*32;
				spr.y = y*32;
				window.draw(spr);				
			}
			if(grid[y][x] == 4)
			{
				spr.setTextureRect({32*3,0,32,32});
				spr.x = x*32;
				spr.y = y*32;
				window.draw(spr);
				
			}
			

		}
	}	
	

	int cx = circle.getPosition().x;
	int cy = circle.getPosition().y;	
	
	int val = grid[cy/32][cx/32];
	if(val == 1 || val == 3 || val == 4)	
	{
		circle.setPosition(125,300);
		//playsound
	}
		
	if(val == 2)
	{
		win = true;
		circle.setPosition(125,300);
	}
				
	
	if(cx/32 > gridWidth)
	{
		circle.setPosition(125,300);
	}

	
}








int main()
{
	
	
	
//	int hightex[gridHeight][gridWidth] = {
//	
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//		
//	
//	};
	
	int hightex[gridHeight][gridWidth] = {
	
		4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
		4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,
		4,4,4,4,4,4,4,4,4,4,4,4,3,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,
		4,4,4,3,3,3,3,4,4,4,4,3,0,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,4,
		4,4,3,0,0,0,0,3,4,4,3,0,0,0,0,0,0,0,3,4,4,4,4,4,4,4,4,4,4,4,
		3,3,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,3,3,3,4,4,4,4,4,4,4,4,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,4,4,4,4,4,3,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,4,4,4,4,4,4,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,3,4,4,4,3,3,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,3,3,3,0,2,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,1,0,0,0,0,4,0,0,0,1,4,1,0,0,0,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,1,4,1,0,0,0,4,0,0,0,4,4,4,1,0,0,0,0,0,0,0,0,
		4,4,1,0,0,1,1,1,4,4,4,1,1,0,4,0,0,0,4,4,4,4,0,0,0,0,0,1,1,1,
		4,4,4,1,1,4,4,4,4,4,4,4,4,1,4,1,1,1,4,4,4,4,1,1,1,1,1,4,4,4,
		4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
		
	
	};	
	

	
	
	
	
	ws::Window carpet(960,540,"carpet");
	
	
	
	ws::Texture texture;
	ws::Sprite back;
	
	texture.load("back.bmp");
	back.setTexture(texture);
	
	
	ws::Radial circle;
	
	circle.make();
	circle.setPosition(125,300);
	
	int direction = 1;
	



	ws::Texture quarttex;
	quarttex.load("spike.bmp");

	for(int x=0;x<quarttex.width;x++)
	{
		for(int y=0;y<quarttex.height;y++)
		{
			if(quarttex.getPixel(x,y) == Gdiplus::Color(255,255,0,237))
				quarttex.setPixel(x,y,Gdiplus::Color(0,0,0,0));
		}
	}


	ws::Sprite quartstile;
	quartstile.setTexture(quarttex);



	ws::View view;
	view.setRect({0,0,960,540})
	view.setPortRect({0,0,960,540});
	
	while(carpet.isOpen())
	{
		
		
			
			
		
		MSG m;
		while(carpet.pollEvent(m))
		{
			if(m.message == WM_LBUTTONDOWN)
			{
				direction = -3;
			}
			else
			{
				direction = 3;
			}
			
			
			
		}
		
		
		
		view.setRotation(view.getRotation() + 0.5);
		
		
		
		circle.setPosition(circle.getPosition().x + 1,circle.getPosition().y + direction);
		
		
		carpet.setView(view);
		
		carpet.clear();
		
		carpet.draw(back);
		
		
		drawLevel(hightex,carpet,quartstile,circle);
		
		
		if(win)
		{
			std::cout << "Win!";
			win = false;
		}
		
		carpet.draw(circle);
		
		
		carpet.display();
		
		
	}
	
	return 0;
}

