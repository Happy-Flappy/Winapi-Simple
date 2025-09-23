#include "graphics.h"




int main()
{
	ws::Window window(960,540,"My Window");
	
	while(window.isOpen())
	{
		
		
		window.clear();
		
		
		window.display();
		
	}
	 
}