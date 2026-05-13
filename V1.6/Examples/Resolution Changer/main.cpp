#include "winsimple.hpp"






int main()
{
	
	ws::Screen screen;
	
	std::vector<ws::Vec2i> modes = screen.getDisplayModes();
	ws::Vec2i start = screen.getSize();
	
	std::cout << "Resolution changing demo: Press Escape to reset. Press down to change to smaller resolutions.\n";
	
	int index = modes.size()-1;
	bool released = true;
	while(true)
	{
		if(ws::Global::getButton(VK_ESCAPE))
		{
			screen.setSize(start.x,start.y);
			system("pause");
			return 0;
		}
		
		if(ws::Global::getButton(VK_DOWN))
		{
			if(released)
			{
				index--;
				if(index < 0)
					index = 0;
				if(screen.setSize(modes[index]) == "INVALID")
				{
					MessageBoxA(NULL,"Invalid resolution!","INVALID",MB_OK);
				}
				std::cout << modes[index].x << ","<< modes[index].y<<"\n";
			}
			released = false;
		}
		else
			released = true;
	}
	screen.setSize(1920,1080);
	return 0;
}