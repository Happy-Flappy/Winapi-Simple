#include "graphics.h"




int main()
{
	
	ws::Window window(960,540,"");

	ws::Button buttonSmaller;
	buttonSmaller.init(window);
	buttonSmaller.setTitle("Smaller");
	buttonSmaller.setSize(100,100);
	
	ws::Button buttonLarger;
	buttonLarger.init(window);
	buttonLarger.setTitle("Larger");
	buttonLarger.setSize(100,100);
	buttonLarger.setPosition(960-100,0);
	
	
	ws::Radial radial;
	radial.setRadius(100);
	radial.setPosition(300,300);
	radial.setFillColor(RGB(255,160,0));
	radial.setBorderColor(RGB(200,130,0));
	
	
	float radius = 100;
	
	while(window.isOpen())
	{
		
		MSG msg;
		
		while(window.pollEvent(msg))
		{
			
			if(buttonSmaller.isPressed(msg))
			{
				radius --;
				radial.setRadius(radius);
			}
			
			if(buttonLarger.isPressed(msg))
			{
				radius ++;
				radial.setRadius(radius);
			}
			
		}
		
		
		window.clear();
		window.draw(radial);
		window.display();
	}
	
	return 0;
}