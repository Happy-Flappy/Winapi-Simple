#include "winsimple.h"



int main()
{
	ws::Window window(960,540,"");
	

	//Example Conversions
	ws::Vec2f v1(1, 2);
    ws::Vec2f v2(1.5, 2.5);
    ws::Vec2f v3(true, false);
    ws::Vec2f v4(1.5f, 2);
    
    ws::Vec2i vi1(1.5f, 2.7f);
    ws::Vec2i vi2(true, 10);
    
    ws::Vec2f vf = vi1;
    ws::Vec2i vi = vf;
	//<><><><><><><><><><>






	ws::Texture texture;
	texture.load("object.bmp");
	ws::Sprite spr;
	spr.setTexture(texture);
	spr.setScale({0.5,0.5});
	
	ws::Button button;
	button.init(window);
	button.setPosition(400,0);
	button.setText("Copy!");
	
	
	
	ws::Rectangle shape;
	shape.x = 400;
	shape.y = 400;
	shape.width = 300;
	shape.height = 300;
	
	
	
	ws::Line line;
	line.start = {700,30};
	line.end = {20,500};
	line.width = 5;
	line.color = Gdiplus::Color(255,255,255,0);
	
	
	
	ws::Poly poly;
	poly.addVertex(0,0);
	poly.addVertex(100,30);
	poly.addVertex(70,100);
	poly.addVertex(40,50);
	poly.filled = true;
	poly.closed = true;
	poly.borderWidth = 3;
	poly.fillColor = Gdiplus::Color(155,255,0,0);
	poly.borderColor = Gdiplus::Color(255,255,0,255);
	
	
	
	std::vector<ws::Radial> rads;
	
	for(int a=0;a<100;a++)
	{
		ws::Radial rad;
		rad.setFillColor(Gdiplus::Color(rand()%255,rand()%255,rand()%255,rand()%255));
		rad.setBorderColor(Gdiplus::Color(rand()%255,rand()%255,rand()%255,rand()%255));
		rad.setPosition(ws::Vec2i(rand()%960,(rand()%540 - 540 - 50)));
		rad.setPointCount(rand()%20);
		rad.setRadius(rand()%50);
		rads.push_back(rad);
	}
	
	
	
	ws::Timer timer;
	double timesincelastupdate = 0;
	double timeperframe = (1.f/60.f);
	
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
				button.update(&m);
				if(button.isPressed(m))
				{
					window.clipboard.copyTexture(window.backBuffer);
				}
	
			}
			
			
		
			for(int a=0;a<rads.size();a++)	
			{
				if(rads[a].getPosition().y - rads[a].getRadius() > 540 + 50)
				{
					rads[a].setFillColor(Gdiplus::Color(rand()%255,rand()%255,rand()%255,rand()%255));
					rads[a].setBorderColor(Gdiplus::Color(rand()%255,rand()%255,rand()%255,rand()%255));
					rads[a].setPosition({rand()%960,(rand()%540 - 540 - 50)});
					rads[a].setPointCount(rand()%20);
					rads[a].setRadius(rand()%50);
				}
				
				rads[a].setPosition({rads[a].getPosition().x,rads[a].getPosition().y + 1});
				
			}
			
		}
		
		
		window.clear();
		window.draw(spr);
		window.draw(shape);
		window.draw(line);
		window.draw(poly);
		
		for(int a=0;a<rads.size();a++)
		{
			window.draw(rads[a]);
		}
		
		window.display();
	}
	return 0;
}