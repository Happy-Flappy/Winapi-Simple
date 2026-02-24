#include "winsimple.h"




int main()
{
	ws::Window window(960,540,"");

	ws::Texture texture;
	ws::Sprite sprite;
	
	ws::DropTarget target;
	target.acceptType("images");
	target.acceptType("text");
	target.acceptType("files");
	
	
	ws::ClipData data;
	ws::DropEffect effect;
	
	while(window.isOpen())
	{
		while(target.pollDrop(data,effect))
		{
			if(data.getTexture().isValid())
			{
				texture = data.getTexture();
				sprite.setTexture(texture,true);
			}
		}
		window.clear();
		window.draw(sprite);
		window.display();
	}
	return 0;
}