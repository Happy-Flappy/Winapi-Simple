#include "graphics.h"



std::vector<POINT> points;



class Player
{
	public:
	
	ws::Line line;
	
	float speed = 1;
	POINT direction = {0,0};
	POINT lastDirection = {-1,0};
	std::vector<POINT> points;
	
	Player(int x,int y)
	{
		line.start = {x,y};
		line.color = RGB(0,0,254);
		
		
		points.push_back({line.start.x,line.start.y});
	}	


	void update()
	{

		bool moveRequest = false;		
		if(ws::Global::getKey(VK_UP))
		{
			direction.x = 0;
			direction.y = -1;
			moveRequest = true;
		}
		if(ws::Global::getKey(VK_DOWN))
		{
			direction.x = 0;
			direction.y = 1;
			moveRequest = true;
		}
		if(ws::Global::getKey(VK_LEFT))
		{
			direction.y = 0;
			direction.x = -1;
			moveRequest = true;
		}
		if(ws::Global::getKey(VK_RIGHT))
		{
			direction.y = 0;
			direction.x = 1;
			moveRequest = true;
		}
		
		if(direction.x != lastDirection.x || direction.y != lastDirection.y)
		{
			
			points.push_back({line.start.x,line.start.y});
			
			lastDirection = direction;
		}
		
		if(moveRequest)
		{
			line.start.x += speed * direction.x;
			line.start.y += speed * direction.y;
		}
		
		//Adjust last point to reach up to 
		if(points.size() > 1)
		{
			points[points.size()-1].x = line.start.x;
			points[points.size()-1].y = line.start.y;
		}
		
		
		
		if(ws::Global::getKey(VK_SPACE))
		{
			points.clear();
			points.push_back({line.start.x,line.start.y});
			lastDirection = {-1,0};
		}
		
	}
	
	void draw(ws::Window &window)
	{
		line.end.x = line.start.x + 3; 
		line.end.y = line.start.y + 5;
		
		window.draw(line);
		line.end.x = line.start.x + 5;
		line.end.y = line.start.y + 3;
		window.draw(line);
	}
	
};




int main()
{
	
	ws::Window window(960,540,"",0);
	window.setChromaKey(RGB(0,0,255));
	window.setLayerAfter(HWND_TOPMOST);
	window.setFullscreen(true);

	
	Player player(window.view.getSize().x/2,window.view.getSize().y/2);



	
	ws::Poly poly;
	poly.filled = false;
	poly.borderColor = RGB(255,0,0); 
	poly.closed = false;
	
	
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
			
			//updates
			
			player.update();
			
			poly.vertices = player.points;
			
			if(ws::Global::getKey(VK_ESCAPE))
			{
				return 0;
			}
			

			
		}
		
		
		
		
		//render
		
		window.clear(RGB(0,0,255));
		window.draw(poly);
		player.draw(window);
		window.display();
	}
	
	return 0;
	
}