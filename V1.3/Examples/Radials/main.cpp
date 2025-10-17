#include "graphics.h"
#include <vector>





int main()
{
	ws::Window window(960,540,"");
	
	
	
	
	
	std::vector<ws::Radial> shapes;
	
	for(int a=0;a<100;a++)
	{
		shapes.push_back(ws::Radial());
		shapes[a].setPosition(rand() % 960,(rand() % 540) - 540 - 50);
		shapes[a].setFillColor(RGB(rand()%255,rand()%255,rand()%255));
		shapes[a].setBorderColor(RGB(rand()%255,rand()%255,rand()%255));
		shapes[a].setBorderWidth(rand() % 5);
		shapes[a].setPointCount(rand()%100);
		
	}
	
	
	
	
	
	ws::Radial circle;
	circle.setRadius(30);
	circle.setPosition(960/2,540/2);
	circle.setFillColor(RGB(0,0,255));
	circle.setBorderColor(RGB(255,0,255));
	circle.setBorderWidth(4);
	
	POINT mouse = {0,0};
	
	
	
	
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
				if(m.message == WM_MOUSEMOVE)
				{
					int mousex = GET_X_LPARAM(m.lParam);
					int mousey = GET_Y_LPARAM(m.lParam);
					mouse = window.view.toWorld({mousex,mousey});
				}
			}
			
			
			
			for(int a=0;a<shapes.size();a++)
			{
				shapes[a].setPosition(shapes[a].getPosition().x,shapes[a].getPosition().y + 10);
				if(shapes[a].getPosition().y > 540 + (shapes[a].getRadius() * 2))
				{
					shapes[a].setPosition(rand() % 960,(rand() % 540) - 540 - 50);
				}
			}
			
		}
		
		window.clear();
		circle.setPosition(mouse);
		
		for(int a=0;a<shapes.size();a++)
		{
			window.draw(shapes[a]);
		}
		
		
		window.draw(circle);
		
		window.display();
	}
	
	
	return 0;
}