#include "winsimple.hpp"
#include "winsimple-controls.hpp"


int main()
{
	ws::Window window(960,540,"");
	
	
	
	ws::Icon icon("logoMarkYellow.ico");
	
	window.setIcon(icon);
	
	while(window.isOpen())
	{
		
		
	}


	system("pause");
	return 0;
}