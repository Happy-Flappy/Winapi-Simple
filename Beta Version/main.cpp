#include "winsimple.hpp"



int main()
{
	bool released = true;
	while(true)
	{
		if(ws::Global::getButton(ws::Key::Snapshot))
		{
			if(released)
			{
				std::cout << "Snapshot Key Pressed! Saving...\n";
				ws::Screen screen;
				screen.getSnapshot().saveToFile("Snapshot.png");
			}
			released = false;
		}
		else
			released = true;
	}
	return 0;
}