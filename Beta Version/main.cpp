#include "winsimple.h"






int main()
{
	
	ws::Window window;
	
	
	ws::Window newwindow;
	
	newwindow.create(960,540,"");
	
	window = newwindow;
	
	while(window.isOpen())
	{
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_KEYDOWN)
			{
				if(m.wParam == VK_ESCAPE)
				{
					window.close();
				}
				if(m.wParam == 'V')
				{
					window.setVisible(false);
				}
				if(m.wParam == 'S')
				{
					window.setVisible(true);
				}
				
				
				
			}
			
		}
		
		
		if(window.hasFocus())
		{
			std::cerr << "Has focus...\n";
		}
		else
		{
			window.setFocus();
			std::cerr << "Forced back into focus.\n";
		}
		
		
		window.clear();
		window.display();
	}
	
}