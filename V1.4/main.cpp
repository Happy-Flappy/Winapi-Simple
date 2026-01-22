#include "winsimple.h"




int main()
{
	ws::Window window(960, 540, "");

	while (window.isOpen())
	{
		window.clear();
		window.display();
	}
}