#include "winsimple.h"








int main()
{
	ws::Window window(1920,1080,"");
	
	ws::Texture texture;
	texture.load("client.bmp");
	
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
	
	while(window.isOpen())
	{
		
		MSG msg;
		while(window.pollEvent(msg))
		{
		}
		

		window.clear(ws::RGBA::Transparent());
		window.draw(sprite);
		window.display();
	}
	
	return 0;
}