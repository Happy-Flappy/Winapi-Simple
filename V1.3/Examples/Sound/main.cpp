#include "graphics.h"







int main()
{
	ws::Window window(960,540,"");
	
	
	ws::Wav music;
	music.open("song15.mid",music.getFreeChannel());
	music.blocking = true;//default - shown for example			
	music.play();
	
	
	
	ws::Radial shape;
	shape.setPosition(320,300);
	shape.setFillColor(RGB(125,100,100));
	shape.setBorderColor(RGB(255,0,0));
	
	
	
	ws::Vec2d velocity = {0,0};
	
	
	
	ws::Timer timer;
	double timesince = 0;
	double timeperframe = 1.f/60.f;
	
	
	while(window.isOpen())
	{
		
		timesince += timer.getSeconds();
		timer.restart();


			MSG m;
			
			while(window.pollEvent(m))
			{
				if(m.message == WM_KEYDOWN)
				{
					if(m.wParam == VK_UP)
					{
						velocity.y = -4;
						ws::Wav::PlayFree("boing.wav",1,true);
						
					}
					if(m.wParam == VK_LEFT)
					{
						velocity.x = -4;
						ws::Wav::PlayFree("woosh.wav",2,true);
					}
					if(m.wParam == VK_RIGHT)
					{
						velocity.x = 4;
						ws::Wav::PlayFree("woosh.wav",3,true);
					}
					
					
				}
			}
			

		
		while(timesince > timeperframe)
		{
			timesince -= timeperframe;
		
			
			

			
			
			if(shape.getPosition().y + shape.getRadius() < 540)
			{
				velocity.y += 0.08;
			}
			else
			{
				velocity.y = 0;
				shape.setPosition(shape.getPosition().x,540 - shape.getRadius() - 1);
				velocity.x *= 0.92;
			}
			
			shape.move(velocity.x,velocity.y);
			
			
		
		}
		
		if(music.isFinished())
			music.setProgress(0);  
		
		window.clear(RGB(100,100,255));
		window.draw(shape);
		window.display();
	}
}