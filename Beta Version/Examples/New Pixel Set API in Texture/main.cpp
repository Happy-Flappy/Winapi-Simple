#include "winsimple.h"




int main()
{
	ws::Window window(960,540,"");
	
	ws::Texture texture;
	texture.create(960,540);
	
	
	
	//EXAMPLE CODES <><><><><><><><><><><><><><><><><><><><><>><><><><><><<>
	
	
	//Pixel Access - By 1D array - FASTER
	for(int a=0;a<texture.width * texture.height;a++)
		texture.setPixel(RGB(rand()%255,rand()%255,rand()%255),a);
	
	//Pixel Access - By 2D array
	for(int x=0;x<texture.width;x++)	
	{
		for(int y=0; y < texture.height;y++)
		{
			texture.setPixel(RGB(rand()%255,rand()%255,rand()%255),x,y);
		}
	}
	
	
	//Pixel Access by 1D to 2D array conversion
	ws::Vec2i index2D = texture.getPixelIndex(5);
	texture.setPixel(RGB(rand()%255,rand()%255,rand()%255),index2D.x,index2D.y);
	
	//Pixel Access by 2D to 1D array conversion
	int index1D = texture.getPixelIndex(10,20);
	texture.setPixel(RGB(rand()%255,rand()%255,rand()%255),index1D);
	
	
	/////////////////////////////////////////////////////////////////////
	
	
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	while(window.isOpen())
	{
		
		
		//Pixel Access - By 1D array - FASTER
		for(int a=0;a<texture.width * texture.height;a++)
			texture.setPixel(RGB(rand()%255,rand()%255,rand()%255),a);
		
		
		window.clear(RGB(0,255,255));
		window.draw(sprite);
		window.display();
	}
	
	return 0;
}