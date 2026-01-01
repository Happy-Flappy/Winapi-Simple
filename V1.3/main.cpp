#include "graphics.h"

void GetPrimaryScreenSize(int& width, int& height) {
    // Get the width of the primary display monitor in pixels
    width = GetSystemMetrics(SM_CXSCREEN);
    // Get the height of the primary display monitor in pixels
    height = GetSystemMetrics(SM_CYSCREEN);
}

int main()
{
	
	int width,height;
	GetPrimaryScreenSize(width,height);
	
	ws::Window window(width,height,"",0);
	
	window.setChromaKey(RGB(255,0,255));
	
	ws::Line lineVert,lineHorizon;
	int size = 10;
	
	lineVert.start.x = width/2;
	lineVert.start.y = (height/2) - (size/2);
	lineVert.end.x = width/2;
	lineVert.end.y = (height/2) + (size/2);
	
	lineHorizon.start.x = (width/2) - (size/2);
	lineHorizon.start.y = (height/2);
	lineHorizon.end.x = (width/2) + (size/2);
	lineHorizon.end.y = (height/2);
	
	
	window.setLayerAfter(HWND_TOPMOST);
	
	window.setFullscreen(true);
	
	while(window.isOpen())
	{
		
		if(ws::Global::getKey(VK_ESCAPE))
		{
			return 0;
		}
		
		
		window.clear(RGB(255,0,255));
		window.draw(lineVert);
		window.draw(lineHorizon);
		window.display();
	}
	return 0;
}