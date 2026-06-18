#include "winsimple.hpp"



int main()
{
	ws::Rectangle rect(300,100);
	rect.setPosition(3,3);
	rect.setBorderWidth(3);
	rect.setBorderColor(ws::Hue::blue);
	rect.setFillColor(ws::Hue::yellow);
	
	ws::Hue hue = ws::Hue::yellow;
	ws::Hue::HSV hsv = hue.toHSV();
	
	
	ws::Window window(960,540,"");
	window.enableChromaKey(ws::Hue::pink);
	
	while(window.isOpen())
	{
		hsv.h+=0.1;
		rect.setBorderColor(hsv.toHue());
		
		window.clear(ws::Hue::pink);
		window.draw(rect);
		window.display();
	}
	return 0;
}