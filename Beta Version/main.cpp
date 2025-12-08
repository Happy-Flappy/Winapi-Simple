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
	sprite.setScale({-1,1});
	
	while(window.isOpen())
	{
		window.clear();
		window.draw(sprite);
		window.display();
	}
	return 0;
}