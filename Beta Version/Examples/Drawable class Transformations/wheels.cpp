#include "winsimple.h"





class Physics
{
	public:
	double x,y;
	int radius;
	ws::Vec2d velocity = {200,0};
	float rotateDirect = 200;
	float rotation = 0;
	bool drag = false;
	
	ws::Vec2f update(double dt)
	{
		
		
		if(x - radius < 0)
		{
			x = 0 + radius;
			velocity.x = -velocity.x;
		}
		
		if(x + radius > 960)
		{
			x = 960 - radius;
			velocity.x = -velocity.x;
		}
		
		if(y + radius > 540)
		{
			if(abs(velocity.y) > 500)
				velocity.y = -velocity.y * 0.9;
			else
				velocity.y = -velocity.y;
			y = 540 - radius;	
		}
		
		velocity.y += 30;//gravity
		
		
		x += velocity.x * dt;
		y += velocity.y * dt;
		
		rotation += rotateDirect * dt;
		
		rotateDirect = velocity.x;
		
		
		return ws::Vec2f(x,y);
	}
	
	

	
	
};	





int main()
{
	
	ws::Window window(960,540,"");



	ws::Texture tex;
	tex.loadFromFile("wheels.png");


	
	ws::Sprite wheel2;
	wheel2.setTexture(tex);
	wheel2.setTextureRect(ws::IntRect(85 * 3,0,85,85));
	wheel2.setOrigin(wheel2.width/2,wheel2.height/2);
	
	
	std::vector<Physics> phy;
	
	
	for(int a=0;a<10;a++)
	{
		Physics p;
		p.radius = wheel2.getVisualWidth()/2;
		
		p.x = rand() % 960;
		p.y = rand() % 300;
		p.rotateDirect = rand() % 100 >= 50 ? -200: 200;
		p.velocity.x = rand() % 300 - 150;
		
		phy.push_back(p);	
	}
	
	
	ws::Timer timer;
	
	ws::Vec2i mouseScreen = {0,0};
	
	while(window.isOpen())
	{
		double dt = timer.getSeconds();
		timer.restart();
		
		
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_MOUSEMOVE)
			{
				int x = GET_X_LPARAM(m.lParam);
				int y = GET_Y_LPARAM(m.lParam);
				mouseScreen = {x,y};
			}
		}
		
		
		
		window.clear(Gdiplus::Color(255,100,200,100));
		
		for(int a=0;a<phy.size();a++)
		{
		
			phy[a].update(dt);
		
			wheel2.setPosition(phy[a].x - wheel2.getVisualWidth()/2,phy[a].y - wheel2.getVisualHeight()/2);
			wheel2.setRotation(phy[a].rotation);
			window.draw(wheel2);
			
			static bool released = true;
			
			ws::Vec2i MPosition = window.getView().toWorld(mouseScreen);
			
			if(wheel2.contains(MPosition))
			{
//				if(ws::Global::getMouseButton(VK_LBUTTON))
//				{
//					if(released)
//						phy[a].drag = !phy[a].drag;
//					released = false;
//				}
//				
				phy[a].velocity.y = -1000;
			}
			
			if(!ws::Global::getMouseButton(VK_LBUTTON))
			{
				released = true;
			}
			
			
			
			if(phy[a].drag)
			{
				phy[a].x = MPosition.x;
				phy[a].y = MPosition.y;
				phy[a].velocity.x = 0;
				phy[a].velocity.y = 0;
			}
			
			
		}
		
		window.display();
	}
	return 0;
}