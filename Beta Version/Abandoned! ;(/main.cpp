#include "winsimple.h"








int main()
{
	ws::Window window(960,540,"");
	
	ws::Texture texture;
	//texture.load("sky.bmp");
	texture.create(960,540);
	texture.clear(ws::BGRA(0,0,255,255));
	
	texture.fillRect(ws::BGRA(255,0,0,80),50,50,100,100);
	
	
	
	
	
	
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
	
	while(window.isOpen())
	{
		
		MSG msg;
		while(window.pollEvent(msg))
		{
			if(msg.message == WM_KEYDOWN)
			{
				if(msg.wParam == VK_ESCAPE)
					return 0;
			}
		}

		window.clear(ws::BGRA(0,0,0,0));
		window.draw(sprite);
		window.display();
	}
	
	return 0;
}