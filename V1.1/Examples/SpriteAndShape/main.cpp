#include "graphics.h"






int main()
{
	ws::Window window(960,540,"");
	
	
	ws::Shape rectangle;
	rectangle.width = 1000;
	rectangle.height = 100;
	rectangle.x = 0;
	rectangle.y = 440;
	rectangle.color = RGB(188,104,0);
	
	ws::Texture tlolly;
	tlolly.load("lolly.bmp");
	
	ws::Sprite lolly;
	lolly.setTexture(tlolly);
	lolly.x = -100;
	lolly.y = 440;
	lolly.setOrigin({0,tlolly.height});
	tlolly.setTransparentMask(RGB(0,0,255));
	
	window.toggleFullscreen();
	
	float y = 0;
	while(window.isOpen())
	{

		lolly.x += 5;
		if(lolly.x > window.view.getSize().x)
			lolly.x = -100;

		y -= 0.1;
		lolly.setOrigin({0,LONG(tlolly.height - y)});
	

		window.clear(RGB(70,230,255));
		window.draw(rectangle);
		window.draw(lolly);
		window.display();
	}
	return 0;
	
}