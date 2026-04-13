#include "winsimple.hpp"


int main()
{
	
	ws::Screen screen;
	
    ws::Window window(screen.getSize().x,screen.getSize().y, "Warp",0,0);
	window.setAllStyle(0);
	window.setAllExStyle(0);
	
	window.addExStyle(WS_EX_TRANSPARENT);
	window.addExStyle(WS_EX_LAYERED);
	window.addStyle(WS_POPUP | WS_VISIBLE);
	
	
	ws::Texture cTex;
	cTex.loadFromFile("icons8-cursor-144.png");
	
	ws::Sprite cursor;
	cursor.setTexture(cTex);
	cursor.setOrigin(51,32);
	
	SetWindowDisplayAffinity(window.hwnd,WDA_EXCLUDEFROMCAPTURE);		
	ws::Timer timer;
	float scale = 1.5;

    while(window.isOpen())
    {
		
		ShowCursor(false);
	
		float dt = timer.restart();
		

		if(ws::Global::getButton(VK_ESCAPE) && (ws::Global::getButton(VK_LCONTROL) || ws::Global::getButton(VK_RCONTROL)))
		{
			return 0;
		}
		
		if((ws::Global::getButton(VK_LCONTROL) || ws::Global::getButton(VK_RCONTROL)) && ws::Global::getButton(VK_UP))
			scale+=dt;
		if((ws::Global::getButton(VK_LCONTROL) || ws::Global::getButton(VK_RCONTROL)) && ws::Global::getButton(VK_DOWN))
			scale-=dt;
		
		if(scale < 1)
			scale = 1;
		if(scale > 20)
			scale = 20;
		
		int srcWidth  = static_cast<int>(window.getSize().x  / scale);
		int srcHeight = static_cast<int>(window.getSize().y / scale);
		if (srcWidth < 1) srcWidth = 1;
		if (srcHeight < 1) srcHeight = 1;		
		
		
		window.setLayerAfter(HWND_TOPMOST);

		
		ws::Texture back = screen.getSnapshot();
		HDC srcDC = back.getHDC();
		HDC dstDC = window.backBuffer.getHDC();
		

		window.clear(ws::Hue::transparent);
		
		

		ws::Vec2f mousePos = ws::Global::getMousePos(window);

        // source rectangle centered at the mouse
        int desiredLeft = static_cast<int>(mousePos.x - srcWidth / 2.0f);
        int desiredTop  = static_cast<int>(mousePos.y - srcHeight / 2.0f);
		
        // Clamp source rectangle to not go outside of screen.
        int screenWidth  = screen.getSize().x;
        int screenHeight = screen.getSize().y;
        int srcLeft = std::max(0, std::min(desiredLeft, screenWidth  - srcWidth));
        int srcTop  = std::max(0, std::min(desiredTop,  screenHeight - srcHeight));

        // relative position of the mouse inside the source rectangle
        float relX = (mousePos.x - srcLeft) / static_cast<float>(srcWidth);
        float relY = (mousePos.y - srcTop)  / static_cast<float>(srcHeight);
        // clamp to [0,1]
        relX = std::max(0.0f, std::min(1.0f, relX));
        relY = std::max(0.0f, std::min(1.0f, relY));

        // destination position for the cursor sprite (within the magnified window)
        float cursorX = relX * window.getSize().x;
        float cursorY = relY * window.getSize().y;
		
		
        StretchBlt(dstDC, 0, 0, window.getSize().x, window.getSize().y, srcDC, srcLeft, srcTop, srcWidth, srcHeight, SRCCOPY);

		cursor.setScale(1,1);
		cursor.setPosition(cursorX,cursorY);
		window.draw(cursor);
		
		window.display(); 
		
		ws::Vec2i c = ws::Global::getMousePos();
		
		if(ws::Global::getButton(VK_LEFT))
			c.x -= dt * 200;
		if(ws::Global::getButton(VK_RIGHT))
			c.x += dt * 200;
		if(ws::Global::getButton(VK_UP) && !ws::Global::getButton(VK_LCONTROL) && !ws::Global::getButton(VK_RCONTROL))
			c.y -= dt * 200;
		if(ws::Global::getButton(VK_DOWN)  && !ws::Global::getButton(VK_LCONTROL) && !ws::Global::getButton(VK_RCONTROL))
			c.y += dt * 200;
		
		SetCursorPos(c.x,c.y);
		
    }
	ShowCursor(true);
    return 0;
}