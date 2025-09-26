#include "graphics.h"




int main()
{
	ws::Window window(960,540,"My Window",WS_OVERLAPPEDWINDOW);
	
	
	window.setChromaKey(RGB(0,0,255),100);
	window.setLayerAfter(HWND_TOPMOST);
	
	
	ws::Shape shape;
	shape.color = RGB(255,0,0);
	shape.width = 100;
	shape.height = 100;
	shape.x = 0;
	shape.y = 0;
	
	float x = 1;
	float y = -1;
	
	while(window.isOpen())
	{
		window.clear(RGB(0,0,255));
		
		
		if(shape.x < 0)
		{
			x = 1;
		}
		if(shape.x > window.view.getSize().x)
		{
			x = -1;
		}
		if(shape.y < 0)
		{
			y = 1;
		}
		if(shape.y > window.view.getSize().y)
		{
			y = -1;
		}
		
		
		
		
		shape.x += x;
		shape.y += y;
		
		window.draw(shape);
		
		window.display();
	}
	
}