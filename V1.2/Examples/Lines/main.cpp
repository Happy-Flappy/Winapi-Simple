#include "graphics.h"






int main()
{
	ws::Window window(960,540,"");
	
	ws::Line line({window.width/2,window.height/2},{0,0},2);
	
	ws::Line other({700,300},{400,500},4,RGB(255,0,0));
	
	
	while(window.isOpen())
	{
	
		POINT mouse = ws::Global::getMousePos(window);
		
		line.end = mouse;
		
		if(line.intersects(other))
		{
			line.color = RGB(200,0,0);
		}
		else
		{
			line.color = RGB(0,0,255);
		}
		
		window.clear(RGB(37,37,37));
		window.draw(line);
		window.draw(other);
		window.display();
	}
	return 0;
}