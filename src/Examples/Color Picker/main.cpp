#include "winsimple.hpp"
#include "winsimple-controls.hpp"


int main()
{
    ws::Window window(960,540, "Warp");

	ws::ClickMenu clickMenu;
	clickMenu.init(window);
	clickMenu.addItem("Choose Color");
	clickMenu.addItem("Pick Color");
	
	ws::ColorDialog getColor;
	getColor.init(window);
	getColor.setInitColor(ws::Hue::green);
	
	ws::Screen screen;
	
	
	ws::Texture sky,ground;
	sky.loadFromFile("sky.png");
	ground.loadFromFile("ground.png");
	
	ws::Sprite sprite;
	
	bool colorPick = false;
	ws::Hue clearColor = ws::Hue::green;
	ws::Texture fluid;
	fluid.create(window.getSize().x,300,ws::Hue::transparent);


    float phase = 0.0f;
    const float frequency = 0.02f;
    const float amplitude = 10.0f;
    const float speed = 0.2f;

    while (window.isOpen())
    {
 		MSG msg;
		while(window.pollEvent(msg))
		{
			if(msg.message == WM_RBUTTONDOWN)
			{
				clickMenu.open(ws::Global::getMousePos(window));
	
				int cmd = clickMenu.getResult();
				
				if(cmd == 1)
				{
					if(getColor.open())
						clearColor = getColor.getResult();
				}
				if(cmd == 2)
				{
					colorPick = true;
				}
				if(cmd == 3)
				{
					screen.getSnapshot().saveToFile("snapshot.png");
				}
			}
			

		}
		
		if(colorPick)
		{
			clearColor = screen.getPixel(ws::Global::getMousePos());
			if(ws::Global::getButton(VK_LBUTTON))
			{
				colorPick = false;
			}
		}
		
		
        window.clear(ws::Hue::cyan);
		
		sprite.setTexture(sky,true);
		sprite.setScale(2,1);
		sprite.setPosition(0,0);
		window.draw(sprite);
		
		phase -= 0.1;
		
		for(int a=0;a<fluid.getSize().x * fluid.getSize().y; a++)
			fluid.setPixel(a,ws::Hue::transparent);
		
		for(int x=0;x<fluid.getSize().x;x++)
		{
            float offset = amplitude * std::sin(x * frequency + phase);
            int yOffset = static_cast<int>(offset);
			
		
			for(int y=0;y<fluid.getSize().y;y++)
			{
				int red = clearColor.r - y + yOffset + (x / 4);
				int green = clearColor.g - y + yOffset + (x / 4);
				int blue = clearColor.b - y + yOffset + (x / 4);
				
				if(red < 0)
					red = 0;
				if(green < 0)
					green = 0;
				if(blue < 0)
					blue = 0;
				
				if(red > 255)
					red = 255;
				if(green > 255)
					green = 255;
				if(blue > 255)
					blue = 255;
				
				fluid.setPixel(x,y + yOffset + 20,ws::Hue(red,green,blue,200));
			}
		}
		
		sprite.setTexture(fluid);
		sprite.setScale(1,1);
		sprite.setPosition(0,540 - 250);
		window.draw(sprite);
		
		
		sprite.setTexture(ground);
		sprite.setScale(1,1);
		sprite.setPosition(-400,0);
		window.draw(sprite);
		
		
        window.display(); 
    }
    return 0;
}