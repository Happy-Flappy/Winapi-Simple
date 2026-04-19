#include "winsimple.hpp"


int main()
{
	ws::Window window(1200,675,"");
	
	ws::Texture texture;
	texture.loadFromFile("clouds.png");
	
	ws::Texture drawTex;
	//drawTex.loadFromFile("");
	
	window.create(texture.getSize().x,texture.getSize().y,"Warp");
	
	
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
	ws::Timer timer;
	
	double amplitude = 5.0;
	double frequency = 0.005;
	double speed = 1.0;
	double time = 0.0;
	
	
	while(window.isOpen())
	{
		float dt = timer.restart();
		
		
		
		window.clear(ws::Hue(169 - 20,173 - 20,150 - 20));
	
	
		for(int y=0;y<texture.getSize().y;y++)
		{
			float offset = amplitude * std::sin(2 * 3.14 * frequency * (y - speed * time));
			for(int x=0;x<texture.getSize().x;x++)
			{
				if(drawTex.getSize().x == 0 || (drawTex.getSize().x > 0 && drawTex.getPixel(x,y) == ws::Hue::blue))
					window.setPixel(x + offset,y,texture.getPixel(x,y));
				else
					window.setPixel(x,y,texture.getPixel(x,y));
			}
			time += dt/14;
		}
		
	
	
		window.display();
	}
	return 0;
}