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
	
	
	while(window.isOpen())
	{
		double dt = timer.getSeconds();
		timer.restart();
		
		
		
		window.clear(Gdiplus::Color(255,100,200,100));
		
		for(int a=0;a<phy.size();a++)
		{
		
			phy[a].update(dt);
		
			wheel2.setPosition(phy[a].x - wheel2.getVisualWidth()/2,phy[a].y - wheel2.getVisualHeight()/2);
			wheel2.setRotation(phy[a].rotation);
			window.draw(wheel2);
			
			
			ws::Vec2i MPosition = window.toWorld(ws::Global::getMousePos(window));
			
			if(wheel2.contains(MPosition))
			{
				phy[a].velocity.y = -1000;
			}
			
			
		}
		
		window.display();
	}
	return 0;
}