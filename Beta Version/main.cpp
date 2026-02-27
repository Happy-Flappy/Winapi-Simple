#include "winsimplenew.h"
#include <iostream>

class Camera
{
	public:
	
	double x,y,z;
	
	int perspective = 240;
	int visible = 600;
	
	
	Camera(double x,double y,double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		
		
	}
	
};



class Map
{
	public:
	
	
	
	
	const int width = 10000;
	const int depth = 10000;
	int dataSize  = 0;
	
	enum Type
	{
		Lolly,
		Shed,
		House,
		Tree,
		Ground
	};
	
	
	
	
	class Data {
	public:
	    float x, y, z;
	    ws::Vec2f size;
	    float scale;
	    Type type = Type::Ground;
	    Data() {}
	};
	
	std::vector<Data> data;
	
	float calculateHillHeight(float x, float z) {
//	    double amplitude1 = 30.0f;
//	    double frequency1 = 0.01f;
//	    double amplitude2 = 30.0f;
//	    double frequency2 = 0.01f;
//	
//	    float y = amplitude1 * std::sin(frequency1 * x) +
//	              amplitude2 * std::cos(frequency2 * z) +
//	              amplitude1 * std::sin(frequency1 * (x + z) * 0.7f);
//	    y += 480.0f;
//	    return y;


	    double amplitude1 = 30.0f;
	    double frequency1 = 0.01f;
	    double amplitude2 = 30.0f;
	    double frequency2 = 0.01f;
	
	    float y = amplitude1 * std::sin(frequency1 * x) +
	              amplitude2 * std::cos(frequency2 * z) +
	              amplitude1 * std::sin(frequency1 * (x + z) * 0.7f);
	    y += 480;
	    float steep = z/4;
		y += z / steep;
	    return y;
		



	}
	
	void makeTerrain() {
		
	    for(float x = 0; x < width; x += 20) 
		{
			for(float z = 0; z < depth; z += 20) {
	            
				
				
				Data d;
	            d.x = x;
	            d.z = z;
	            d.y = calculateHillHeight(x, z);
	            
	            data.emplace_back(d);
	            
	        }


	    }
	    
	    dataSize = data.size();
	    
	    
		std::sort(data.begin(),data.end(),[](Data &a,Data &b){
			return a.z > b.z;
		});
		
		
		
		for(int a=0;a<dataSize;a += rand() % 300)
		{
			if(a < dataSize)
			{
				data[a].type = Type::Tree;
			}
		}

		for(int a=0;a<dataSize;a += rand() % 1200)
		{
			if(a < dataSize)
			{
				data[a].type = Type::Shed;
			}
		}		
		
		for(int a=0;a<dataSize;a += rand() % 1200)
		{
			if(a < dataSize)
			{
				data[a].type = Type::Lolly;
			}
		}
	    
	}
	
	
	
	
	
	
	
	
	
}map;


class Player
{
	
	public:
	
	
	ws::Vec3f velocity;
	
	
	ws::Sprite sprite;
	ws::Texture texture;
	
	Player()
	{
		texture.loadFromFile("blupi.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(ws::IntRect(248,93,61,61));
		sprite.setOrigin(sprite.getTextureRect().width/2,sprite.getTextureRect().height + (sprite.getTextureRect().height/4));
		sprite.setScale(2,2);
		
	}
	
	
	
	void update(Camera &camera)	
	{
		
		//hill adds to gravity based on slope
		
		float h1 = map.calculateHillHeight(camera.x,camera.z);
		float h2 = map.calculateHillHeight(camera.x,camera.z+1);
		
		
		velocity.z -= (h1-h2)/15;
		
		
		float h3 = map.calculateHillHeight(camera.x,camera.z);
		float h4 = map.calculateHillHeight(camera.x+1,camera.z);
		
		
		velocity.x -= (h3-h4)/15;
		
		
		
		
	}
	
	
	void draw(ws::Window &window)
	{
		sprite.setPosition(window.getSize().x/2,window.getSize().y);
		//window.draw(sprite);
	}
	
};















int main()
{
	ws::Window window(960,540,"");
	ws::Vec2f windowSize = {960.0f,540.0f};
	
	
	
	Camera camera(map.width/2,0,100);
	
	map.makeTerrain();
	
	ws::Radial shape;
	shape.setFillColor(ws::Hue(255,255,255,255));
	shape.setBorderWidth(0);
	shape.setRadius(30);
	shape.setOrigin(shape.getRadius(),0);
	shape.setPointCount(5);
	
	
	
	ws::Timer clock;
	double timesincelastupdate = 0;
	double timeperframe = 1.f/60.f;
	
	ws::Sprite sprite;
	ws::Texture texture;
	texture.loadFromFile("explo.png");
	sprite.setTexture(texture);
	sprite.setTextureRect(ws::IntRect(0,1361,92,125));
	sprite.setOrigin(sprite.getTextureRect().width/2,sprite.getTextureRect().height);
	
	Player player;
	
	
	
	ws::Timer timer;
	
	texture.setPixel(10,10,ws::Hue::red);
	
	std::cerr << timer.getSeconds();
	
	while(window.isOpen())
	{
		
		timesincelastupdate += clock.getSeconds();
		clock.restart();
		
		while(timesincelastupdate > timeperframe)
		{
			
			timesincelastupdate -= timeperframe;
			
			
			window.clear(ws::Hue::cyan);
			
			
			camera.z += player.velocity.z;
			
			camera.x += player.velocity.x;
			
			
	        // Camera movement
	        if(ws::Global::getButton(VK_UP)) 
	            player.velocity.z += 0.05;
	        if(ws::Global::getButton(VK_DOWN)) 
	            player.velocity.z -= 0.05;
	        if(ws::Global::getButton(VK_LEFT)) 
				player.velocity.x -= 0.05;
	        if(ws::Global::getButton(VK_RIGHT))
	        	player.velocity.x += 0.05;
	        
		}
		
		camera.y = map.calculateHillHeight(camera.x,camera.z) - 50;
		
		for(int a=0;a<map.data.size();a++)
		{
			
			Map::Data &d = map.data[a];
			
			float relativeZ = d.z - camera.z;
			float relativeX = d.x - camera.x;
			float relativeY = d.y - camera.y;

			if(relativeZ < camera.visible/2 && a == 0)
			{
				camera.z = 100;
			}
			
        	if (relativeZ < 0 || relativeZ > camera.visible || relativeX < -camera.visible || relativeX > camera.visible || relativeY < -camera.visible || relativeY > camera.visible) continue;
			
			float scale = camera.perspective / relativeZ;
			
			float screenX = (relativeX * scale) + windowSize.x/2;
			float screenY = (relativeY * scale) + windowSize.y/2;
			
			shape.setPosition(screenX,screenY);
			shape.setScale(scale,scale);

			float height = d.y - 255;
			if(height > 255)
				height = 255;
			if(height < 0)
				height = 0;
			
			shape.setFillColor(ws::Hue(height,height,height));
			shape.setBorderColor(ws::Hue(height,height,height));
			window.draw(shape);
			
			if(d.type == Map::Type::Tree)
				sprite.setTextureRect(ws::IntRect(0,1361,92,125));
			if(d.type == Map::Type::Shed)
				sprite.setTextureRect(ws::IntRect(0,437,127,95));
			if(d.type == Map::Type::Lolly)
				sprite.setTextureRect(ws::IntRect(368,995,56,121));
			
			
			
			if(d.type != Map::Type::Ground)
			{
			
				sprite.setOrigin(sprite.getTextureRect().width/2,sprite.getTextureRect().height - 20);
				sprite.setPosition(screenX,screenY);
				sprite.setScale(scale,scale);
				
				window.draw(sprite);
			}
			
			
			
		}
		player.update(camera);
		player.draw(window);
		
		window.display();
	}
	
	
	return 0;
	
}