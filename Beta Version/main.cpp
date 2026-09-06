#include "winsimple.hpp"

int main()
{
	ws::Screen screen;
	ws::Window window(screen.getSize().x,screen.getSize().y,"",0,0);
	window.setAllStyle(0);
	window.setAllExStyle(0);
	
	window.addExStyle(WS_EX_TRANSPARENT);
	window.addExStyle(WS_EX_LAYERED);
	window.addStyle(WS_POPUP | WS_VISIBLE);

	ws::Texture canvas;
	canvas.create(screen.getSize().x,screen.getSize().y);
	ws::Sprite sprite(canvas);


	const uint8_t GREEN_R = 80;    // slight red for warm phosphor
	const uint8_t GREEN_G = 220;
	const uint8_t GREEN_B = 40;

	static const int bayer[4][4] = {
		{ 0,  8,  2, 10 },
		{12,  4, 14,  6 },
		{ 3, 11,  1,  9 },
		{15,  7, 13,  5 }
	};


	SetWindowDisplayAffinity(window.hwnd, WDA_EXCLUDEFROMCAPTURE);
	
	ws::Timer timer;
	
	while(window.isOpen())
	{
		window.setLayerAfter(HWND_TOPMOST);
		//window.clear();
		
		timer.restart();

		canvas = screen.getSnapshot();

        
        int w = screen.getSize().x;
        int h = screen.getSize().y;
        int totalPixels = w * h;

        canvas.editAllPixels([&](uint8_t* p,int index){
            int x = index % w;
            int y = index / w;

            uint8_t b = p[0];
            uint8_t g = p[1];
            uint8_t r = p[2];

            int lum = static_cast<int>(r * 0.299f + g * 0.587f + b * 0.114f);

            // Dithering threshold from Bayer matrix
            int rx = x & 3;          // x % 4
            int ry = y & 3;          // y % 4
            int threshold = bayer[ry][rx] * 16 + 8;   // 0..255

            if(lum > threshold) 
			{
                p[0] = GREEN_B;
                p[1] = GREEN_G;
                p[2] = GREEN_R;
            } 
			else 
			{
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
            }			
		});
		
		sprite.Blt(window.backBuffer);
		
		window.display(); 
	}
	return 0;
} 