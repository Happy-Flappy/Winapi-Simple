#include "winsimple.h"




int main()
{
	
	ws::Window window(960,540,"");
	
	
	ws::Texture texture;
	texture.create(100,100,ws::Hue(255,0,0));
	
	
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
	while(window.isOpen())
	{
		
		ws::Hue hue = ws::Hue::yellow;
		
		Gdiplus::Color gdiColor;
		gdiColor = hue;
		
		
		window.clear(hue);
		window.draw(sprite);
		window.display();
	}
}