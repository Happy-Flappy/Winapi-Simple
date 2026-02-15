#include "winsimple.h"



void set(int xpos,int ypos,ws::Texture &tex,double a,int size,ws::Hue color)
{
	int x = xpos + static_cast<int>(std::sin(a) * float(size));
	int y = ypos + static_cast<int>(std::cos(a) * float(size));
	tex.setPixel(x,y,color);
}



class Loader
{
	public:
	
	double current = 0;
	ws::Hue newColor;
	int x,y;
	float speed;
	
	Loader()
	{
		current = rand()%static_cast<int>(3.14*2);
		newColor = ws::Hue(rand()%255,rand()%255,rand()%255,rand()%255);
		speed = rand()% 10;
		x = rand()%960;
		y = rand()%540;
	}
	
	void update(double dt,ws::Texture &texture)
	{
		current += dt * ((speed+1)/10);
		
		
		if(current > 3.14*2)
		{
			current = 0;
			newColor = ws::Hue(rand()%255,rand()%255,rand()%255);
		}
		
		for(int b=0;b<30;b++)
			set(x,y,texture,current,50 - b,newColor);	
			
	}	
		
};


int main()
{
	
	ws::Window window(960,540,"");
	
	
	
	ws::Texture texture;
	texture.create(960,540,ws::Hue::brown);
	
	
	int amount = 1000;
	Loader loaders[amount];//they modify the texture
	
	
	

	
	
	ws::Timer timer;

	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	while(window.isOpen())
	{
		double dt = timer.getSeconds();
		timer.restart();
		
		
		for(int a=0;a<amount;a++)
		{
			loaders[a].update(dt,texture);
		}
		
		
		window.clear();
		window.draw(sprite);
		window.display();
	}
}