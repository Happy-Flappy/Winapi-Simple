#include "graphics.h"




void multiplyColor(COLORREF apply,ws::Texture &texture)
{
	ws::PixelArray pixels;
	
	pixels.convertToPixel(texture);	
	
	for(int x=0;x<pixels.width;x++)
	{
		for(int y=0;y<pixels.height;y++)
		{
			COLORREF c = pixels.getPixel(x,y);
			int r = GetRValue(c);
			int g = GetGValue(c);
			int b = GetBValue(c);
			pixels.setPixel(RGB(r+GetRValue(apply),g+GetGValue(apply),b+GetBValue(apply)),x,y);
		}
	}
	
	pixels.sendToTexture(texture);
}




int main()
{
	ws::Texture texture;
	texture.load("lolly.bmp");
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
		

	ws::PixelArray originalPixels;
	
	ws::PixelArray pixels;
	pixels.convertToPixel(texture);
	
	originalPixels = pixels;
	
	
	
	ws::Window window(960,540,"");
	
	float direct = 2;
	float r = 0;
	
	
	ws::Timer timer;
	double timeperframe = 1.f/60.f;
	double lastTime = 0;
	
	while(window.isOpen())
	{
		
		
		lastTime += timer.getSeconds();
		timer.restart();
		
		while(lastTime > timeperframe)
		{
			lastTime -= timeperframe;
		
			r+=direct;
			if(r <= 0)
			{
				direct = 2;
				r = 0;
			}
			if( r >= 255)
			{
				direct = -2;
				r = 255;
			}	
		
		}
		
		pixels = originalPixels;
		pixels.setMask(RGB(0,0,255),RGB(static_cast<int>(r),0,0));
		pixels.sendToTexture(texture);
		
		
		window.clear();
		window.draw(sprite);
		window.display();
	}
	return 0;
}