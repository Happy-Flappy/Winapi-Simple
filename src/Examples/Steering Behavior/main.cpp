#include "winsimple.hpp"
#include "winsimple-controls.hpp"


double getMagnitude(ws::Vec2f v)
{
	return std::sqrt(v.x*v.x+v.y*v.y);
}

void setMagnitude(ws::Vec2f &v,double targetMag)
{
	double currentMag = std::sqrt(v.x*v.x+v.y*v.y);
	
	if(currentMag == 0)
		return;
	
	double scale = targetMag / currentMag;
	v.x *= scale;
	v.y *= scale;
}


struct Pixel
{
	Pixel(ws::Hue hue,ws::Vec2f target,ws::Vec2f start)
	{
		this->hue = hue;
		this->target = target;
		this->pos = start;
	}
	
	ws::Hue hue;
	ws::Vec2f pos,vel,acc,target;
	float maxSpeed = 2;
	float maxForce = 0.3;
	
	void applyForce(ws::Vec2f force)
	{
		acc.x += force.x;
		acc.y += force.y;
	}
	
	ws::Vec2f seek(ws::Vec2f t)
	{
		ws::Vec2f desired = {t.x - pos.x,t.y - pos.y};
		double distance = getMagnitude(desired);
		
		//if 0 to 100 is 0 to maxSpeed then 100 is 100%.
		float scale = distance/100;
		float speed = maxSpeed * scale;
		
		setMagnitude(desired,speed);
		
		ws::Vec2f steer = {desired.x - vel.x,desired.y - vel.y};
		return steer;		
	}
	

	ws::Vec2f flee(ws::Vec2f t)
	{
		//seek the target
		
		ws::Vec2f desired = {t.x - pos.x,t.y - pos.y};
		double distance = getMagnitude(desired);
		
		if(distance >= 50)
			return ws::Vec2f(0,0);
		
		setMagnitude(desired,maxSpeed);
		
		desired.x *= -2;
		desired.y *= -2;
		
		ws::Vec2f steer = {desired.x - vel.x,desired.y - vel.y};
		return steer;
	}
	
	void update(ws::Vec2f MPosition)
	{
		pos.x += vel.x;
		pos.y += vel.y;
		
		vel.x += acc.x;
		vel.y += acc.y;
		
		acc = {0,0};
		
		ws::Vec2f steering = seek(target);
		if(steering.x > maxForce)
			steering.x = maxForce;
		if(steering.y > maxForce)
			steering.y = maxForce;
		
		applyForce(steering);
		
		
		
		
		applyForce(flee(MPosition));
	}
};



class Image
{
	public:
	ws::Texture texture;
	std::vector<Pixel> pixels;
	int stride = 1;
	
	void load(std::string path)
	{
		texture.loadFromFile(path);
		texture.setSize(960,540);
		
		float ratio = static_cast<float>(50000) / (texture.getSize().x * texture.getSize().y);
        stride = static_cast<int>(std::sqrt(1.0f / ratio));
        if (stride < 1) stride = 1;
		
		pixels.clear();
		for(int x=0;x<texture.getSize().x;x+=stride)
		{
			for(int y=0;y<texture.getSize().y;y+=stride)
			{
				//{rand()%960,rand()%540}
				ws::Hue h = texture.getPixel(x,y);
				if(h.a != 0)
					pixels.push_back(Pixel(h,{x,y},{x+((rand()%800)-400),y + ((rand()%800)-400)}));
				
			}
		}
	}
	
	void update(ws::Vec2f MPosition)
	{
		for(auto& p : pixels)
		{
			p.update(MPosition);
		}
		
		if(ws::Global::getButton(VK_SPACE))
		{
			for(auto& p : pixels)
			{
				p.pos.x = rand()%960;
				p.pos.y = rand()%960;
				
			}
		}
	}
	
	void draw(ws::Window &window)
	{
		for(auto& p : pixels)
		{
			for(int x=0;x<stride+1;x++)
			{
				for(int y=0;y<stride+1;y++)
				{
					window.setPixel(p.pos.x+x,p.pos.y+y,p.hue);
				}
			}
		}
	}
};



int main()
{
	ws::Window window(960,540,"Steering Behavior");
	
	ws::FileWindow fwindow;
	
	Image image;
	
	if(!fwindow.open())
		return 0;
	else
		image.load(fwindow.getResult()); 
	
	
	
	ws::Timer timer;
	float timesincelastupdate = 0;
	float timeperframe = 1.f/60.f;
	
	while(window.isOpen())
	{
		timesincelastupdate += timer.restart();
		while(timesincelastupdate > timeperframe)
		{
			ws::Vec2f MPosition = window.toWorld(ws::Global::getMousePos(window));
			
			image.update(MPosition);
			timesincelastupdate -= timeperframe;
		}
		window.clear();
		image.draw(window);
		window.display();
	}
	return 0;
}