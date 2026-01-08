#include "winsimple.h"




int main()
{
	
	ws::Window window(960,540,"");
	
	ws::Texture tex;
	tex.loadFromFile("spike.bmp");
	ws::Sprite spr;
	spr.setTexture(tex);
	spr.x = 300;
	spr.y = 300;
	
	spr.setOrigin(spr.getTextureRect().width/2,spr.getTextureRect().height/2);
	spr.setScale(-1,4);
	
	
	ws::Radial rad;
	rad.setPosition(400,200);
	rad.setRadius(60);
	rad.setFillColor(Gdiplus::Color(rand()%255,rand()%255,rand()%255,rand()%255));
	rad.setBorderColor(Gdiplus::Color(rand()%255,rand()%255,rand()%255,rand()%255));
	rad.setBorderWidth(3);
	rad.setOrigin(rad.getPosition().x,rad.getPosition().y);
	
	double r = 0;
	
	while(window.isOpen())
	{
		spr.setRotation(r);
		rad.setRotation(r);
		r += 1;
		window.clear();
		window.draw(spr);
		window.draw(rad);
		window.display();
	}
	return 0;
}