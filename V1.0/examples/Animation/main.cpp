#include "graphics.h"
#include "camera.h"
#include "shift.h"

#include <vector>



int main()
{
	

	
	ws::Window window(1920,1080,"My Window");
	
	
	
	ws::Texture texture; 
	texture.load("explo.bmp");
	texture.setTransparentMask(RGB(0,0,255));



	ShiftData shift;
	
	
	shift.delay = 0.15;
	shift.rect.push_back({237,437,120,102});
	shift.rect.push_back({127,1237,110,123});
	shift.rect.push_back({360,1237,119,124});
	shift.rect.push_back({335,342,124,95});
	shift.rect.push_back({324,761,99,116});
	shift.rect.push_back({230,877,106,117});
	
	
	ws::Shape shape;
	shape.color = RGB(255,255,0);
	shape.setTexture(texture);
	shape.fitTexture();
	shape.rect = {237,437,120,102};
	shape.setOrigin(shape.rect.right/2,shape.rect.bottom/2);	


	
	
	
	
	

	
	
	
	int mouseX,mouseY;
	
	
	while(window.isOpen())
	{
	
		MSG e;
		while(window.pollEvent(e))
		{
		
			if(e.message == WM_MOUSEMOVE)
			{
				mouseX = GET_X_LPARAM(e.lParam);
				mouseY = GET_Y_LPARAM(e.lParam);
				
			}
			
			if(e.message == WM_KEYDOWN)
			{
				
			}
		}
		
		
		window.clear(RGB(37,37,37));
		
		shape.x = mouseX;
		shape.y = mouseY;
		
	
		
		int left = Shift(shift).left;
		int top = Shift(shift).top;
		int width = Shift(shift).width;
		int height = Shift(shift).height;
		
		shape.rect = {left,top,width,height};
		
		
		
		
		window.draw(shape);
		
		window.display();
	}
	
	return 0;
	
	
}