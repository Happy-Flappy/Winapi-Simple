#include "winsimple.hpp"

int main()
{
	ws::Window window(960,540,"");
	ws::Icon ico("logoMarkYellow.ico");
	window.setIcon(ico);
	
	while(window.isOpen())
	{
		window.clear();
		window.display(); 
	}
	return 0;
}