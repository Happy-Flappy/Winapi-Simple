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
	


    float phase = 0.0f;
    const float frequency = 0.02f;
    const float amplitude = 5.0f;
    const float speed = 0.2f;

	SetWindowDisplayAffinity(window.hwnd,WDA_EXCLUDEFROMCAPTURE);

	ws::Timer timer;

    while(window.isOpen())
    {
		
		float dt = timer.restart();
		
		phase -= dt;

		if(ws::Global::getButton(VK_ESCAPE) && (ws::Global::getButton(VK_LCONTROL) || ws::Global::getButton(VK_RCONTROL)))
		{
			return 0;
		}
 
		window.setLayerAfter(HWND_TOPMOST);


		ws::Texture back = screen.getSnapshot();
		
		
		HDC srcDC = back.getHDC();
		HDC dstDC = window.backBuffer.getHDC();

		int width  = back.getSize().x;
		int height = back.getSize().y;

		window.clear(ws::Hue::transparent);
		
		static std::vector<float> sinTable;
		static std::vector<float> cosTable;
		static int lastWidth = 0;
		
		if (width != lastWidth) 
		{
			sinTable.resize(width);
			cosTable.resize(width);
			for (int x = 0; x < width; x++) {
				float angle = x * frequency;
				sinTable[x] = std::sin(angle);
				cosTable[x] = std::cos(angle);
			}
			lastWidth = width;
		}

		// Pre‑compute phase trig values once per frame
		float sinPhase = std::sin(phase);
		float cosPhase = std::cos(phase);		
		
		
		//use bitblt to copy entire rows instead of shifting each pixel for the wave.
		for (int x = 0; x < width; x++)
		{
			float offset = amplitude * (sinTable[x] * cosPhase + cosTable[x] * sinPhase);
			int yOffset = static_cast<int>(offset);

			BitBlt(dstDC, x, yOffset, 1, height, srcDC, x, 0, SRCCOPY);
		}

		window.display(); 
    }
    return 0;
}