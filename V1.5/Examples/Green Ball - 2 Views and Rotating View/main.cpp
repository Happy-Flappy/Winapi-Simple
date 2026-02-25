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
	
	
	
//	int level0[gridHeight][gridWidth] = {
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
	
	int level0[gridHeight][gridWidth] = {
	
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
	

	
	
	
	
	ws::Window window(960,540,"window");
	window.setFullscreen(true);
	
	
	ws::Texture texture;
	ws::Sprite back;
	
	texture.loadFromFile("back.bmp");
	back.setTexture(texture);
	
	
	ws::Radial circle;
	
	circle.make();
	circle.setPosition(125,300);
	
	
	int direction = 2;
	



	ws::Texture tileTex;
	tileTex.loadFromFile("spike.bmp");

	for(int x=0;x<tileTex.getSize().x;x++)
	{
		for(int y=0;y<tileTex.getSize().y;y++)
		{
			if(tileTex.getPixel(x,y).GetValue() == Gdiplus::Color(255,255,0,237).GetValue())
				tileTex.setPixel(x,y,Gdiplus::Color(0,0,0,0));
		}
	}


	ws::Sprite tileSpr;
	tileSpr.setTexture(tileTex);



	ws::View view;
	view.init(0,0,960,540);//sets both port and world to this value.
	
	ws::View view2;
	view2.setRect(0,0,960,540);
	view2.setPortRect(0,0,960/4,540/4);
	
	ws::Timer timer;
	double timesincelastupdate = 0;
	double timeperframe = 1.f/60.f;
	
	while(window.isOpen())
	{
		
		timesincelastupdate += timer.getSeconds();
		timer.restart();	


		
		
		while(timesincelastupdate > timeperframe)	
		{
			timesincelastupdate -= timeperframe;
		

			MSG m;
			while(window.pollEvent(m))
			{
				if(m.message == WM_LBUTTONDOWN)
				{
					direction = -2;
				}
				else
				{
					direction = 2;
				}
				
				if(m.message == WM_KEYDOWN)
				{
					
					if(m.wParam == VK_UP)
					{
						view.setZoom(view.getZoom() + 0.01);
					}
					
					if(m.wParam == VK_DOWN)
					{
						view.setZoom(view.getZoom() - 0.01);
					}
					
					if(m.wParam == VK_ESCAPE)
					{
						window.close();
					}
					
					
				}
			
					
				
			}
			
			
			view.setPortRotatePointCenter();
			view.setRotation(view.getRotation() + 2);
			//view.setZoom(1.5);
			static float zoomDir = 0.5;
			if(view.getZoom() > 1)
				zoomDir = -0.05;
			if(view.getZoom() < 0.1)
				zoomDir = 0.05;
			view.setZoom(view.getZoom() + zoomDir);
			
			circle.setPosition(circle.getPosition().x + 1,circle.getPosition().y + direction);
			view.setCenter(circle.getPosition());
		}
		
		window.setView(view);
		
		window.clear();
		
		window.draw(back);
		
		
		
		drawLevel(level0,window,tileSpr,circle);
		
		
		if(win)
		{
			std::cout << "Win!";
			win = false;
		}
		
		window.draw(circle);
		
		
		
		
		window.setView(view2);
		
		window.draw(back);
		
		
		
		drawLevel(level0,window,tileSpr,circle);
		
		
		window.draw(circle);		
		
		window.display();
		
		
	}
	
	return 0;
}

