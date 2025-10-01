#include "graphics.h"
#include <vector>



int main()
{
	
	
	std::vector<std::vector<RECT>> tile; 
	
	
	POINT tileSize = {16,16};
	int width = 960;
	int height = 540;
	float scale = 2;
	
	tile.resize(width / tileSize.x);
	
	
	for(int x=0; x < width/tileSize.x; x ++)
	{
		for(int y = 0; y < height/tileSize.y; y ++)
		{
			tile[x].push_back({0,0,0,0});
		}
	}
	
	ws::Window window(width,height,"",0);
	window.setFullscreen(true);
	window.setChromaKey(RGB(86,170,144)); 
	window.setLayerAfter(HWND_TOPMOST); 
	 
	ws::Texture texture;
	texture.load("tileset.bmp"); 
	texture.setTransparentMask(RGB(86,170,144)); 
	 
	ws::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setScale({scale,scale});
	
	
	
	

	bool draw = false;
	
	RECT rect = {0,0,0,0};
	
	rect.right = tileSize.x;
	rect.bottom = tileSize.y;	
		
	while(window.isOpen())
	{
		POINT mouse = ws::Global::getMousePos(window);
		
		
		
		
		
		if(ws::Global::getKey(VK_ESCAPE))
			return 0;
		
		
		
		if(ws::Global::getKey('Q'))
		{
			rect.left = 273;
			rect.top = 273;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		if(ws::Global::getKey('W'))
		{
			rect.left = 290;
			rect.top = 273;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		if(ws::Global::getKey('E'))
		{
			rect.left = 307;
			rect.top = 273;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		
		
		
		if(ws::Global::getKey('A'))
		{
			rect.left = 273;
			rect.top = 290;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		if(ws::Global::getKey('S'))
		{
			rect.left = 290;
			rect.top = 290;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		if(ws::Global::getKey('D'))
		{
			rect.left = 307;
			rect.top = 290;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		
		
		
		
		
		
		if(ws::Global::getKey('Z'))
		{
			rect.left = 273;
			rect.top = 307;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		if(ws::Global::getKey('X'))
		{
			rect.left = 290;
			rect.top = 307;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		if(ws::Global::getKey('C'))
		{
			rect.left = 307;
			rect.top = 307;
			rect.right = tileSize.x;
			rect.bottom = tileSize.y;
		}
		
		
		
		
		
		if(ws::Global::getKey(VK_SPACE))
		{
			rect.right = 0;
			rect.bottom = 0;
		}
		
		
		if(ws::Global::getKey(VK_LEFT))
		{
			window.view.move({10,0});
		}
		if(ws::Global::getKey(VK_RIGHT))
		{
			window.view.move({-10,0});
		}
		
		
		
		
		if(ws::Global::getMouseButton(VK_LBUTTON))
		{
		
			POINT pos;
			pos.x = mouse.x / (tileSize.x * scale);
			pos.y = mouse.y / (tileSize.y * scale);
			
			if(pos.x < tile.size() && pos.x >= 0  && pos.y < tile[0].size() && pos.y >= 0)
				tile[pos.x][pos.y] = rect;				
		
		}
		
		
		
		window.clear(RGB(86,170,144));
		
		for(int x=0;x<tile.size();x++)
		{
			for(int y=0;y<tile[x].size();y++)
			{
				sprite.setTextureRect(tile[x][y]);
				sprite.x = x * tileSize.x * 2;
				sprite.y = y * tileSize.y * 2;
				window.draw(sprite);
			}
		}
		window.display();
	}
	
	return 0;
}