#include "graphics.h"




int main()
{
	ws::Window window(960,540,"");
	
	
	
	
	
	while(window.isOpen())
	{
		
		
		if(ws::Global::getKey(VK_SPACE))
		{
			ws::wav.PlayFile("pzucchin.wav",0,true);
			ws::wav.PlayFile("item_in.wav",1,true);
	
		}
		
		window.clear();
		window.display();
	}
}