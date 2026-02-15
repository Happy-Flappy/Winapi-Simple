#include "winsimple.h"





int main()
{
	ws::Window window(960,540,"");
	DragAcceptFiles(window.hwnd,TRUE);
	
	
	
	//ws::ClipData clip = ws::clipboard.paste();
	ws::ClipData dropData;
	
	
	//ws::Texture texture = clip.getTexture();
	
	//ws::Sprite sprite;
	//sprite.setTexture(texture);
	
	while(window.isOpen())
	{
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_DROPFILES)
				dropData = ws::mover.get(m);
		}
		
		/*
		for(int a=0;a<dropData.getFiles().size();a++)
		{
			std::cout << dropData.getFiles()[a] << "\n";
			system("pause");
			return 0;
		}
		*/
		
		
		window.clear();
		//window.draw(sprite);
		window.display();
	}
	return 0;
	
}