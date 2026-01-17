#include "winsimple.h"

#include <cmath>


int main()
{
	ws::ShiftData shift;
	shift.add(0,0,85,85);
	shift.add(85,0,85,85);
	shift.add(85 * 2,0,85,85);
	shift.add(85 * 3,0,85,85);
	
	
	ws::Texture texture;
	texture.loadFromFile("wheels.png");
	
	ws::Sprite sprite;
	sprite.setTexture(texture);
	
	
	
	
	ws::Window window;
	window.create(960,540,"");
	
	while(window.isOpen())
	{
		window.clear();
		sprite.setTextureRect(ws::Shift(shift));
		sprite.setOrigin(sprite.width/2,sprite.height/2);
		sprite.setRotation(sprite.rotation += 2);
		window.draw(sprite);
		window.display();
	}
	return 0;
}