#include "winsimple.h"




int main()
{
	
	ws::Window window(1920,1080,"");
	ws::Font font;
	
	font.loadFromSystem("Comic Sans MS");
	
	// ֎۞༗႟↭⌾⍬
	
	ws::Text text;
	text.setFont(font);
	
	std::string str = "Testing a string. Testing @ Character.\nTesting Odd Unicode ©haracters - © ® ™ € £ ¥ — – • … « » ♠ ♣ ♥ ♦ ★ ☆ ☺ ☹";
	text.setString(str);

	text.setPosition(0,100);
	text.setBorderColor(Gdiplus::Color(255,100,200,100));
	text.setBorderWidth(2);
	text.setStyle(Gdiplus::FontStyleBoldItalic);
	text.setCharacterSize(30);
	
	
	
	while(window.isOpen())
	{
		
		if(ws::Global::getKey(VK_ESCAPE))
			window.close();
		
		static bool released = true;
		if(ws::Global::getKey(VK_UP) && released)
		{
			text.setBorderWidth(text.getBorderWidth()+1); 
			released = false;
		}
		if(ws::Global::getKey(VK_DOWN) && released)
		{
			text.setBorderWidth(text.getBorderWidth()-1); 
			released = false;
		}
		if(!ws::Global::getKey(VK_UP) && !ws::Global::getKey(VK_DOWN))
			released = true;
		
		
		if(text.contains(ws::Global::getMousePos(window)))
		{
			text.setFillColor(Gdiplus::Color(255,0,0,255));
		}
		else
		{
			text.setFillColor(Gdiplus::Color(255,255,0,0));
		}
		
		window.clear(Gdiplus::Color(255,0,255,255));
		window.draw(text);
		window.display();
	}
	return 0;
}