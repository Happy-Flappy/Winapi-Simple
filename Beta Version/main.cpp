#include "winsimple.h"



int main()
{
	ws::Window window(960,540,"");
	
	ws::Texture texture;
	texture.load("object.bmp");
	ws::Sprite spr;
	spr.setTexture(texture);
	spr.setScale({0.5f,0.5f});
	
	ws::Button button;
	button.init(window);
	button.setPosition(400,0);
	button.setText("Button");
	
	
	
	ws::Rectangle shape;
	shape.x = 400;
	shape.y = 400;
	shape.width = 300;
	shape.height = 300;
	
	
	
	ws::Line line;
	line.start = {700,30};
	line.end = {20,500};
	line.width = 5;
	line.color = Gdiplus::Color(255,255,255,0);
	
	
	
	ws::Poly poly;
	poly.addVertex(0,0);
	poly.addVertex(100,30);
	poly.addVertex(70,100);
	poly.addVertex(40,50);
	poly.filled = true;
	poly.closed = true;
	poly.borderWidth = 3;
	poly.fillColor = Gdiplus::Color(155,255,0,0);
	poly.borderColor = Gdiplus::Color(255,255,0,255);
	
	
	while(window.isOpen())
	{
		MSG m;
		while(window.pollEvent(m))
		{
			if(m.message == WM_LBUTTONDOWN)
			{
				window.clipboard.copyTexture(window.backBuffer);

			}
		}
		button.update(&m);
		window.clear();
		window.draw(spr);
		window.draw(shape);
		window.draw(line);
		window.draw(poly);
		window.display();
	}
	return 0;
}