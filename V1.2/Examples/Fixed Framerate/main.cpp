#include "graphics.h"














int main()
{
	
	ws::Window window(960,540,"My Window");
	
	
	
	
	ws::Shape shape;
	shape.color = RGB(255,0,0);
	shape.width = 100;
	shape.height = 100;
	shape.x = 0;
	shape.y = 0;
	
	float x = 1;
	float y = -1;
	
	
	
	
	ws::Timer timer;
	
	
	int frameCount = 0;
	
	
	
	double timeperframe = 1.f/60.f;
	double timesincelastupdate = 0;
	
	while(window.isOpen())
	{
		
		timesincelastupdate += timer.getSeconds();
		timer.restart();
		
		
		while(timesincelastupdate > timeperframe)
        {
        	timesincelastupdate -= timeperframe;
        	
        	
			
			
			if(shape.x < 0)
			{
				x = 5;
			}
			if(shape.x > window.view.getSize().x)
			{
				x = -5;
			}
			if(shape.y < 0)
			{
				y = 5;
			}
			if(shape.y > window.view.getSize().y)
			{
				y = -5;
			}
			
			
			
			
			shape.x += x;
			shape.y += y;
		}
		
		window.clear(RGB(37,37,37));

		window.draw(shape);
		
		window.display();
		
	}
	
}