#include "winsimple.h"






int main()
{
	
	ws::Window window;
	
	
	ws::Texture backTex;
	ws::Sprite sprite;
	backTex.load("back2.jpg");
	sprite.setTexture(backTex);
	sprite.setOrigin(0,sprite.getTextureRect().height);
	sprite.setScale(1,-1);
	
	
	window.create(backTex.width,backTex.height,"");
	window.setFullscreen(true);
	
	while(window.isOpen())
	{
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_KEYDOWN)
			{
				if(m.wParam == VK_ESCAPE)
				{
					window.close();
				}
				
				
			}
			
		}
		
		
		
		window.clear();
		window.draw(sprite);
		window.display();
	}
	
}