#include "winsimple.h"


int main()
{
	ws::Window window(960,540,"");
	ws::Radial circle;
	circle.setFillColor(Gdiplus::Color(255,100,200,100));
	circle.setPosition(960/2,540/2);
	circle.setRadius(200);
	while(window.isOpen())
	{
		window.clear();
		window.draw(circle);
		window.display();
	}
	return 0;
}