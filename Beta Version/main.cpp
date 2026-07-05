#include "winsimple.hpp"
#include "winsimple-controls.hpp"


int main()
{
	ws::Window window(960,540,"");
	window.setIcon("logoMarkYellow.ico");
	
	
	ws::Balloon(window,"Winsimple Says you have Windows " + ws::getWindowsVersion() + "!","Winsimple");
	
	while(window.isOpen())
	{
		
		
	}


	system("pause");
	return 0;
}