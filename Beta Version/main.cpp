#include "winsimple.h"
#include <iostream>

class Camera
{
	public:
	
	double x=0,y=0,z=0;
	float yaw = 0;
	
	int perspective = 300;
	int visible = 800;
	
	
	Camera(double x,double y,double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	bool project(float x,float y,float z,float &screenX,float &screenY,float &scale)
	{
		float relativeZ = z - this->z;
		float relativeX = x - this->x;
		float relativeY = y - this->y;
		
        float rotX = relativeX * std::cos(yaw) - relativeZ * std::sin(yaw);
        float rotZ = relativeX * std::sin(yaw) + relativeZ * std::cos(yaw);
		relativeX = rotX;
		relativeZ = rotZ;
		
		if (relativeZ < 0 || relativeZ > visible || relativeX < -visible || relativeX > visible || relativeY < -visible || relativeY > visible) return false;
		
		scale = perspective / relativeZ;
		
		screenX = (relativeX * scale) + 960/2;
		screenY = (relativeY * scale) + 540/2;	

		return true;		
	}
};



class Map
{
	public:
	
	const int width = 10000;
	const int depth = 10000;
	const int density = 20;
	
	class Data 
	{
		public:
	    float x, y, z;
		ws::Hue hueID = ws::Hue(255,255,255,255);
	    Data() {}
	};
	
	std::vector<Data> data;
	
	float getHillHeight(float x, float z) 
	{
	    double amplitude1 = 30.0f;
	    double frequency1 = 0.01f;
	    double amplitude2 = 30.0f;
	    double frequency2 = 0.01f;
	
	    float y = amplitude1 * std::sin(frequency1 * x) +
	              amplitude2 * std::cos(frequency2 * z) +
	              amplitude1 * std::sin(frequency1 * (x + z) * 0.7f);
	    y += 480;
	    return y;
	}
	
	void makeTerrain() 
	{
	    for(float x = 0; x < width; x += density) 
		{
			for(float z = 0; z < depth; z += density) 
			{	
				Data d;
	            d.x = x;
	            d.z = z;
	            d.y = getHillHeight(x, z);
	            data.emplace_back(d);
	        }
	    }
		std::sort(data.begin(),data.end(),[](Data &a,Data &b){
			return a.z > b.z;
		});
	}
	
	void populate(std::string path)
	{
		ws::Texture tex;
		tex.loadFromFile(path);
		for(int a=0;a<tex.getSize().x * tex.getSize().y;a++)
		{
			if(a < data.size() && a >= 0)
			{
				data[a].hueID = tex.getPixel(a).GetValue();
			}
		}
	}
	
}map;


class Player
{
	public:
	
	ws::Vec3f velocity = {0,0,0};
	
	Player() {}
	
	void update(Camera &camera)	
	{
		velocity.x *= 0.922;
		velocity.z *= 0.922;
	}
};



#include <algorithm>


ws::Texture blupiTex;

class Blupi
{
public:
	
	float x = 0, y = 0, z = 0;
	float runspeed = 1.5;
	float walkspeed = 0.5;
    ws::Sprite sprite;
	float baseScale = 0.25;
	
	
	ws::ShiftData *currentShift = nullptr;
	
	struct WalkShift
	{
		ws::ShiftData right,bottomright,bottom,bottomleft,left,topleft,top,topright;
		
		WalkShift()
		{
			double animSpeed = 0.001;
			
			right.delay = animSpeed;
			bottomright.delay = animSpeed;
			bottom.delay = animSpeed;
			bottomleft.delay = animSpeed;
			left.delay = animSpeed;
			topleft.delay = animSpeed;
			top.delay = animSpeed;
			topright.delay = animSpeed;
			
			for(int a=0;a<24;a++)
				right.rect.push_back({240 * a,0,240,240});
			for(int a=0;a<24;a++)
				bottomright.rect.push_back({240 * a,240,240,240});
			for(int a=0;a<24;a++)
				bottom.rect.push_back({240 * a,240 * 2,240,240});
			for(int a=0;a<24;a++)
				bottomleft.rect.push_back({240 * a,240*3,240,240});
			for(int a=0;a<24;a++)
				left.rect.push_back({240 * a,240*4,240,240});
			for(int a=0;a<24;a++)
				topleft.rect.push_back({240 * a,240*5,240,240});
			for(int a=0;a<24;a++)
				top.rect.push_back({240 * a,240*6,240,240});
			for(int a=0;a<24;a++)
				topright.rect.push_back({240 * a,240*7,240,240});
		}
	}walkshift;
	
	// No mapIndex — Blupi is no longer inserted into map.data
	
    Blupi(float startX, float startY, float startZ)
    {
		sprite.setTexture(blupiTex);
		currentShift = &walkshift.bottom;
		
		x = startX;
		y = startY;
		z = startZ;
    }
	
/* 	void follow(double newx, double newz)
	{
		float dx = newx - x;
		float dz = newz - z;
		float rad = std::atan2(dx, dz) * 180.0f / 3.14f;
		moveAngle(rad,);		
	} */
	
	float angleRange = 22.5;
	bool inRange(float angle, float strict)
	{
		return angle > strict - angleRange && angle < strict + angleRange;
	}
	
	void moveAngle(float angle,Camera &camera)
	{
		angle = std::fmod(angle, 360.0f);
		if(angle < 0) angle += 360.0f;
		
		if(state == WALKING)
		{
			float rad = angle * 3.14 / 180;
			x += std::sin(rad) * walkspeed;
			z += std::cos(rad) * walkspeed;		
		}
		if(state == RUNNING)
		{
			float rad = angle * 3.14 / 180;
			x += std::sin(rad) * runspeed;
			z += std::cos(rad) * runspeed;			
		}
		
		float cameraYawDeg = camera.yaw * 180.0f / 3.14f;
		float screenAngle = angle - cameraYawDeg;
		screenAngle = std::fmod(screenAngle, 360.0f);
		if(screenAngle < 0) screenAngle += 360.0f;

		bool atTop = (screenAngle > 360 - angleRange/2 || screenAngle < angleRange/2);
		if(atTop)
		{
			if(currentShift != &walkshift.top) currentShift = &walkshift.top;
		}
		else
		{
			if(inRange(screenAngle,45))  { if(currentShift != &walkshift.topright)    currentShift = &walkshift.topright; }
			if(inRange(screenAngle,90))  { if(currentShift != &walkshift.right)       currentShift = &walkshift.right; }
			if(inRange(screenAngle,135)) { if(currentShift != &walkshift.bottomright) currentShift = &walkshift.bottomright; }
			if(inRange(screenAngle,180)) { if(currentShift != &walkshift.bottom)      currentShift = &walkshift.bottom; }
			if(inRange(screenAngle,225)) { if(currentShift != &walkshift.bottomleft)  currentShift = &walkshift.bottomleft; }
			if(inRange(screenAngle,270)) { if(currentShift != &walkshift.topleft)     currentShift = &walkshift.topleft; }
		}
	}
	
	enum State { RUNNING,WALKING, WAITING } state = WAITING;
	ws::Timer runTimer;
	ws::Timer stateTimer;
	double stateDuration = 0;
	float wanderAngle = 0;
	
    void update(Camera &camera)
    {
		
		//bahavior
		
		//if camera in blupi's range of sight.
		//run away!
		float sightRange = 100;

		float dx = camera.x - x;
		float dz = camera.z - z;
		float distSq = dx*dx + dz*dz;

		if(distSq < sightRange * sightRange)
		{
			// Camera is within sight — run away (opposite direction)
			runTimer.restart();
		}		
		
		if(runTimer.getSeconds() < 3)
		{
			state = RUNNING;
			float fleeAngle = std::atan2(dx, dz) * 180.0f / 3.14f;
			fleeAngle += 180.0f;
			moveAngle(fleeAngle,camera);
			y = map.getHillHeight(x, z);			
		}
		else
		{
			if(stateTimer.getSeconds() > stateDuration)
			{
				// Transition state
				if(state == WALKING)
				{
					state = WAITING;
					stateDuration = (rand() % 4) + 1;  // wait 1-4 seconds
				}
				else
				{
					state = WALKING;
					wanderAngle   = rand() % 360;
					stateDuration = (rand() % 3) + 1;  // walk 1-3 seconds
				}
				stateTimer.restart();
			}

			if(state == WALKING)
			{
				moveAngle(wanderAngle, camera);
				y = map.getHillHeight(x, z);
			}
		}
		if(state != WAITING)
			sprite.setTextureRect(ws::Shift(*currentShift));
        sprite.setOrigin(sprite.getTextureRect().width * 0.5f,
                         sprite.getTextureRect().height * 0.85f);
    }
};


int main()
{
	srand(time(nullptr));
	
	ws::Window window(960,540,"");
	ws::Vec2f windowSize = {960.0f,540.0f};
	
	Camera camera(map.width/2,0,100);
	
	map.makeTerrain();
	map.populate("map.bmp");
	
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

	blupiTex.loadFromFile("ASSETS/blupi.png");	
	std::vector<Blupi> blupis;
	blupis.reserve(50);
	for(int a=0;a<50;a++)
	{
		blupis.emplace_back(rand()%map.width,0,rand() %map.depth);
	}
	
	
	static float angle = 0;
	static ws::Timer moveTimer;
	
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
			
	        if(ws::Global::getButton(VK_LEFT))  camera.yaw -= 0.02;
	        if(ws::Global::getButton(VK_RIGHT)) camera.yaw += 0.02;
			
			if(ws::Global::getButton('W'))    
			{
				player.velocity.x += std::sin(camera.yaw) * 0.5;
				player.velocity.z += std::cos(camera.yaw) * 0.5;
			}
			if(ws::Global::getButton('S'))  
			{
				player.velocity.x -= std::sin(camera.yaw) * 0.5;
				player.velocity.z -= std::cos(camera.yaw) * 0.5;
			}	
	        if(ws::Global::getButton('A')) 
			{
				player.velocity.x -= std::sin(camera.yaw + (90 * 3.14/180)) * 0.5;
				player.velocity.z -= std::cos(camera.yaw + (90 * 3.14/180)) * 0.5;				
			}
	        if(ws::Global::getButton('D'))
			{
				player.velocity.x -= std::sin(camera.yaw - (90 * 3.14/180)) * 0.5;
				player.velocity.z -= std::cos(camera.yaw - (90 * 3.14/180)) * 0.5;					
			}
			
			if(moveTimer.getSeconds() > 1)
			{
				angle += 1;
				moveTimer.restart();
			}

			for(auto& blupi : blupis)
			{
				blupi.y = map.getHillHeight(blupi.x, blupi.z);
				blupi.update(camera);
			}

			player.update(camera);	
		}
		
		camera.y = map.getHillHeight(camera.x,camera.z) - 50;
		
		// --- Build unified render list: terrain points + Blupis ---
		// Each item is either a terrain point (Data*) or a Blupi index,
		// unified via a small struct sorted by camera depth.

		struct RenderItem
		{
			float depth;               // camera-space Z for sorting
			Map::Data* terrain;        // non-null if terrain point
			Blupi*     blupi;          // non-null if Blupi
		};

		std::vector<RenderItem> renderList;
		renderList.reserve(10000 + blupis.size());

		// Add visible terrain points
		for(auto& d : map.data)
		{
			float wx = d.x - camera.x;
			float wz = d.z - camera.z;
			float cz = wx * std::sin(camera.yaw) + wz * std::cos(camera.yaw);
			float cx = wx * std::cos(camera.yaw) - wz * std::sin(camera.yaw);
			if(cz > 0 && cz < camera.visible && cx > -camera.visible && cx < camera.visible)
				renderList.push_back({cz, &d, nullptr});
		}

		// Add visible Blupis
		for(auto& blupi : blupis)
		{
			float wx = blupi.x - camera.x;
			float wz = blupi.z - camera.z;
			float cz = wx * std::sin(camera.yaw) + wz * std::cos(camera.yaw);
			float cx = wx * std::cos(camera.yaw) - wz * std::sin(camera.yaw);
			if(cz > 0 && cz < camera.visible && cx > -camera.visible && cx < camera.visible)
				renderList.push_back({cz, nullptr, &blupi});
		}

		// Sort back-to-front
		std::sort(renderList.begin(), renderList.end(), [](const RenderItem& a, const RenderItem& b){
			return a.depth > b.depth;
		});

		// --- Draw ---
		for(auto& item : renderList)
		{
			if(item.terrain)
			{
				Map::Data& d = *item.terrain;
				float screenX, screenY, scale;
				if(!camera.project(d.x, d.y, d.z, screenX, screenY, scale)) continue;

				shape.setPosition(screenX, screenY);
				shape.setScale(scale, scale);

				float height = d.y - 255;
				if(height > 255) height = 255;
				if(height < 0)   height = 0;
				
				shape.setFillColor(ws::Hue(height,height,height));
				shape.setBorderColor(ws::Hue(height,height,height));
				window.draw(shape);

				if(d.hueID == ws::Hue(255,255,255,255)) continue; // plain terrain, no sprite

				if(d.hueID == ws::Hue(0,255,0,255))   sprite.setTextureRect(ws::IntRect(0,1361,92,125));
				if(d.hueID == ws::Hue(0,0,0,255))     sprite.setTextureRect(ws::IntRect(0,437,127,95));
				if(d.hueID == ws::Hue(0,0,255,255))   sprite.setTextureRect(ws::IntRect(368,995,56,121));

				sprite.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height - 20);
				sprite.setPosition(screenX, screenY);
				sprite.setScale(scale, scale);
				window.draw(sprite);
			}
			else if(item.blupi)
			{
				Blupi& blupi = *item.blupi;
				float screenX, screenY, scale;
				if(!camera.project(blupi.x, blupi.y, blupi.z, screenX, screenY, scale)) continue;

				blupi.sprite.setPosition(screenX, screenY);
				blupi.sprite.setScale(blupi.baseScale * scale, blupi.baseScale * scale);
				window.draw(blupi.sprite);
			}
		}

		window.display();
	}
	
	return 0;
}