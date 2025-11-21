#include "graphics.h"







class Canvas
{
	public:
	
	
	int width = 960,height = 540;
	
	
	struct Pixel3D
	{
		int z = INT_MAX;
		COLORREF color = RGB(0,0,0);
	}pixels[960 * 540];
	
	ws::Texture texture;
	ws::Sprite sprite;
	
	Canvas()
	{
		
		texture.load("blank.bmp");
		
	}



	int getIndex(int x,int y)
	{
        if(x >= 0 && x < width && y >= 0 && y < height)
            return y * width + x;
        return -1;
	}

	void setPixel(COLORREF color,int x,int y,int z)
	{
		int index = getIndex(x,y);
		
		
		if(pixels[index].z >= z && index > 0)
		{
			//replace with closer pixel
			pixels[index].color = color;
			pixels[index].z = z;
		}
		
	}
	
	
	bool isOpen(ws::Window &window)
	{
		return window.isOpen();
	}
	
	void clear(ws::Window &window)
	{
		window.clear();
		for(int a=0;a<960*540;a++)
		{
			pixels[a].color = RGB(0,0,0);
		}
	}

	
	
	
	void display(ws::Window &window)
	{
		
		for(int a=0;a<960*540;a++)
		{
			int x,y;
			x = a / width;
			y = a % width;
			texture.setPixel(pixels[a].color,x,y);
		}
		
		
		
		sprite.setTexture(texture);
		window.draw(sprite);
		
		window.display();
	}
		
};






void drawLine(int x0,int y0,int z0,int x1,int y1,int z1,COLORREF color,Canvas &canvas,int width = 2)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int dz = z1 - z0;
	
	
	int s1 = std::max(abs(dx),std::abs(dy));
	float step = std::max(abs(s1),abs(dz));
	
	
	if(step != 0)
	{
		float mx = dx/step; //slope
		float my = dy/step;
		float mz = dz/step;
		
		for(int a=0; a <= step; a++)
		{
            for(int b = 0; b < width; b++)
            {
                for(int c = 0; c < width; c++)
                {
                    int x = static_cast<int>(x0 + a * mx) + b - width/2;
                    int y = static_cast<int>(y0 + a * my) + c - width/2;
                    int z = static_cast<int>(z0 + a * mz);
                    canvas.setPixel(color, x, y, z);
                }
            }
		}
	}
	
}


class Triangle
{
	public:
	
	
	COLORREF color = RGB(255,0,0);
	
	int x1,x2,x3;
	int y1,y2,y3;
	int z1,z2,z3;
	
	
	Triangle(ws::Vec3d p1,ws::Vec3d p2,ws::Vec3d p3)
	{
		x1 = static_cast<int>(p1.x);
		x2 = static_cast<int>(p2.x);
		x3 = static_cast<int>(p3.x);
		y1 = static_cast<int>(p1.y);
		y2 = static_cast<int>(p2.y);
		y3 = static_cast<int>(p3.y);
		z1 = static_cast<int>(p1.z);
		z2 = static_cast<int>(p2.z);
		z3 = static_cast<int>(p3.z);
	}
	
	Triangle()
	{
		
	}
	
	
	void draw(Canvas &canvas)
	{
		
		
		
		//clockwise winding
		drawLine(x1,y1,z1,x2,y2,z2,color,canvas);
		drawLine(x2,y2,z2,x3,y3,z3,color,canvas);
		drawLine(x3,y3,z3,x1,y1,z1,color,canvas);
		
	}
		
};























int main()
{
	ws::Window window(960,540,"");
	Canvas canvas;
	
	
	Triangle tri;
	
	tri = Triangle({0,0,0},{200,200,0},{0,200,0}); 
	
	
	while(canvas.isOpen(window))
	{
		if(ws::Global::getKey(VK_UP))
		{
			tri.z2 ++;
		}
		if(ws::Global::getKey(VK_DOWN))
		{
			tri.z2 --;
		}
		
		
		canvas.clear(window);
		
		tri.draw(canvas);
		
		canvas.display(window);
		
	}
	
	
}