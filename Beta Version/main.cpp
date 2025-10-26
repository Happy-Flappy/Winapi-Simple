#include "winsimple.h"




int main()
{
	ws::Window window(960,540,"");
	
	ws::Radial radial;
	radial.setRadius(500);
	
	
	ws::Input myInput;
	
	while(window.isOpen())
	{
		
		
		myInput = window.input;//This option if you want to pass the input data to a function instead of the entire window.
		
		
		ws::Vec2i mouse = myInput.getPosition(); 
		mouse = window.view.toWorld(mouse); //Vec2i going into function that expects POINT. New operators in Vec2i struct allows this.
		
		
		//POINT going into function expecting Vec2i
		POINT p = mouse;
		radial.setPosition(p);
		
		
		int difference = 0;
		
		if(window.input.Key(VK_UP,true))
			difference = 50;
		if(window.input.Key(VK_DOWN,true))
			difference = -50;
		
		radial.setRadius(radial.getRadius() + difference);
		
		
		window.clear();
		window.draw(radial);
		window.display();
	}
	
	
	
	
	
}