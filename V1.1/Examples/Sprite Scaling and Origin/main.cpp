#include "graphics.h"
#include "shift.h"









class Cursor
{
	public:
	
	ws::Texture texture;
	ws::Sprite lolly;
	ws::Texture tdecor;
	ws::Sprite decor;
	
	float scale = 1;
	
	float sizeDir = 0.01;
	
	ShiftData shift;
	
	Cursor()
	{
	
		texture.load("lolly.bmp");
		texture.setTransparentMask(RGB(0,0,255));
		

	
	
		lolly.setTexture(texture);
		lolly.setOrigin({lolly.getTextureRect().right/2,lolly.getTextureRect().bottom/2});
	
	
		tdecor.load("explo.bmp");
		tdecor.setTransparentMask(RGB(0,0,255));
	
	
	
		decor.setTexture(tdecor);
		decor.setTextureRect({237,437,120,102});
	
		shift.delay = 0.15;
		shift.rect.push_back({237,437,120,102});
		shift.rect.push_back({127,1237,110,123});
		shift.rect.push_back({360,1237,119,124});
		shift.rect.push_back({335,342,124,95});
		shift.rect.push_back({324,761,99,116});
		shift.rect.push_back({230,877,106,117});

		
		
	}
	
	
	void update(POINT &mouse)
	{
		lolly.x = mouse.x;
		lolly.y = mouse.y;
		lolly.setScale({scale,scale});	
		
		
		if(scale > 2)
			sizeDir = -0.01;
		if(scale < 0.1)
			sizeDir = 0.01;	
			
		scale += sizeDir;	
		
		
		
		RECT r;
		
		r.left = Shift(shift).left;
		r.top = Shift(shift).top;
		r.right = Shift(shift).width;
		r.bottom = Shift(shift).height;
		
		
		
		decor.setTextureRect(r);
		decor.x = lolly.x;
		decor.y = lolly.y;
		decor.setOrigin({decor.getTextureRect().right/2,decor.getTextureRect().bottom/2});
		decor.setScale({scale,scale});
			
	}
	
	
	
	
	
	
	void draw(ws::Window &window)
	{
		window.draw(lolly);
		window.draw(decor);	
	}
	
		
};













int main()
{
	ws::Window window(960,540,"");
	
	window.setFullscreen(true);
	
	
	
	ws::Texture tsky;
	tsky.load("sky.bmp");
	ws::Sprite sky;
	sky.setTexture(tsky);	
	
	
	Cursor cursor;
	
	
	
	
	
	while(window.isOpen())
	{
		POINT mouse;
		
		
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_MOUSEMOVE)
			{
				int mouseX = GET_X_LPARAM(m.lParam);
				int mouseY = GET_Y_LPARAM(m.lParam);
				
				mouse = window.view.toWorld({mouseX,mouseY});
				
			}
			
			if(m.message == WM_KEYDOWN)
			{
				if(m.wParam == VK_ESCAPE)
					return 0;
			}
		}
		
		
		
		
			
		cursor.update(mouse);		
		
		
		
		window.clear(RGB(215,215,0));


		window.draw(sky);		
		cursor.draw(window);
		
		
		window.display();
	}
	
}