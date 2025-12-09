#include "winsimple.h"



int main()
{
	ws::Window window(960,540,"");
	
	ws::Texture texture;
	texture.load("object.bmp");
	texture.setTransparentMask(RGB(0,0,255));
	ws::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setTextureRect({465,623,62,64});
	sprite.x = 100;
	sprite.y = 100;
	
	
		ws::Vec2f scale = {1,1};
		POINT origin = {0,0};	
	
	while(window.isOpen())
	{
		

		
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_KEYDOWN)
			{
				if(m.wParam == VK_LEFT)
				{
					scale.x = -1;
				}
				if(m.wParam == VK_RIGHT)
				{
					scale.x = 1;
				}
				if(m.wParam == VK_UP)
				{
					scale.y = -1;
				}
				if(m.wParam == VK_DOWN)
				{
					scale.y = 1;
				}
				
				if(m.wParam == 'A')
				{
					origin.x = 0;
				}
				if(m.wParam == 'D')
				{
					origin.x = sprite.getTextureRect().right;
				}
				if(m.wParam == 'W')
				{
					origin.y = 0;
				}
				if(m.wParam == 'S')
				{
					origin.y = sprite.getTextureRect().bottom;
				}
				
				
			}
		}
		
		sprite.setOrigin(origin);
		sprite.setScale(scale);
		
		window.clear();
		window.draw(sprite);
		window.display();
	}
	return 0;
}