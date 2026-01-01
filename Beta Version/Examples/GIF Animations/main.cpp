#include "winsimple.h"


int main()
{
	
	ws::Window window(960,540,"");
	
	window.addExStyle(WS_EX_LAYERED);
	
	SetLayeredWindowAttributes(window.hwnd,RGB(255,0,255),0,LWA_COLORKEY);
	
	window.setLayerAfter(HWND_TOPMOST);
	
	ws::Animate gif[4];
	
	gif[0].loadFromFile("source.gif");
	gif[1].loadFromFile("mole.gif");
	gif[2].loadFromFile("word.gif");
	gif[3].loadFromFile("planet.gif");
	
	
	
	
	
	ws::Sprite sprite;
	
	
	
	ws::View view;
	view.init(0,0,960/2,540/2);
	
	
	while(window.isOpen())
	{
		window.setView(view);
		window.clear(Gdiplus::Color(255,255,0,255));
		for(int a=0;a<4;a++)
		{
			sprite.setTexture(gif[a].update());
			if(a == 0)
			{
				sprite.x = 0;
				sprite.y = 0;
			}
			if(a == 1)
			{
				sprite.x = 300;
				sprite.y = 0;
			}
			if(a == 2)
			{
				sprite.x = 0;
				sprite.y = 200;
			}
			if(a == 3)
			{
				sprite.x = 200;
				sprite.y = 200;
			}
			window.draw(sprite);
		}
		window.display();
	}
	return 0;
}