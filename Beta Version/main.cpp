#include "winsimple.h"





int main()
{
	ws::Window window(960,540,"");
	window.setFullscreen();
	window.addStyle(CS_DBLCLKS);
	
	ws::Texture texture;
	texture.loadFromFile("quakka2.png");
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
	ws::ClickMenu clickMenu;
	clickMenu.setList({"Option 1","Copy to file","Load into Memory","Option 4","Another option in this list menu but this one is long."});
	clickMenu.init(window);
	
	
	ws::Menu menu;
	menu.setWindow(window);
	
	
	ws::Dropdown drop(1000,"File");
	drop.addItem(1001,MF_STRING,"Item 1");
	drop.addItem(1002,MF_STRING,"Item 2");
	drop.addItem(1003,MF_STRING,"Item 3");
	drop.addItem(1004,MF_STRING,"Item 4");
	
	
	menu.addDropdown(drop);
	
	
	while(window.isOpen())
	{
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_LBUTTONDOWN)
			{
				clickMenu.show(ws::Global::getMousePos(window));
			}
		}
		window.clear(ws::Hue(37,37,37));
		window.draw(sprite);
		window.display();
	}
	return 0;
}