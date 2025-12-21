#include "winsimple.h"
#include <fstream>


std::vector<char> loadIntoMemory(std::string file_path)
{
    std::ifstream file(file_path, std::ios::binary | std::ios::ate); // Open at end to get size

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        std::vector<char> empty;
        return empty;
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg); // Return to start

    std::vector<char> buffer(size);

    file.read(buffer.data(), size);

    file.close();
    
    return buffer;
}

int main()
{
	
	ws::Window window;
	
	
	ws::Texture backTex,backTex2;
	ws::Sprite sprite,sprite2;
	
	backTex.loadFromFile("back2.jpg");

	std::vector<char> data = loadIntoMemory("back.bmp");
	
	backTex2.loadFromMemory(data.data(),data.size());
	
	
	sprite.setTexture(backTex);
	sprite2.setTexture(backTex2);
	sprite2.y = 300;
	
	window.create(backTex.getSize().x,backTex.getSize().y,"");
	//window.setFullscreen(true);
	
	ws::View view,view2;
	
	view = window.getView();
	
	view2.setRect({0,0,backTex.getSize().x,backTex.getSize().y});
	view2.setPortRect(0,0,backTex.getSize().x/2,backTex.getSize().y/2);


	for(int a=0;a<backTex2.getSize().x * backTex2.getSize().y;a++)
	{
		backTex2.setPixel(a,Gdiplus::Color(255,rand()%255,rand()%255,rand()%255));
	}
	
	
	while(window.isOpen())
	{
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_KEYDOWN)
			{
				if(m.wParam == VK_ESCAPE)
				{
					window.close();
				}
				
				
			}
			
		}
		
		view2.setZoom(view2.getZoom() + 0.03);
		
		

		
		window.clear();
		window.setView(view);
		window.draw(sprite);
		window.draw(sprite2);
		window.setView(view2);
		window.draw(sprite);
		window.draw(sprite2);
		window.display();
	}
	
}