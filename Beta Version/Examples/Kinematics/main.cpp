#include "winsimple.hpp"
#include "winsimple-controls.hpp"

class Arm : public ws::Line
{
	public:
	float angle = 0;
	float length = 1;
	
	void getEnd()
	{
		end = {start.x + std::cos(angle) * length,start.y + std::sin(angle) * length};
	}
	
};

class Leg
{
	public:
	Arm parts[4];
	float phase = 0;
	float amplitude = 1.0f;//the effect range size. - multiplied against sine wave of -1 to 1.
	float baseAngle = (3.14/2);//default is down for the leg line direction.
	bool faceLeft = false;

	void init(ws::Vec2f hinge,float length = 40,float phaseOffset = 0)
	{
		phase = phaseOffset;		
	
		//leg
		parts[0].angle = baseAngle;//down
		parts[0].start = hinge;
		parts[0].length = length;
		parts[0].getEnd();
		
		//toes
		parts[1].angle = (parts[0].angle - 3.14/2);//right
		parts[1].start = parts[0].end;
		parts[1].length = 15;
		parts[1].getEnd();
		
		parts[3] = parts[2] = parts[1];
		
		for(int a=0;a<4;a++)
		{
			parts[a].getEnd();
			parts[a].color = ws::Hue::black;
		}
		update(0.0f);		
	}
	
	void update(float time)
	{
		parts[0].angle = baseAngle + std::sin(time + phase) * amplitude;
		
		parts[0].getEnd();
		
		
		//toes
		if(!faceLeft)
			parts[1].angle = parts[0].angle - 3.14 / 2;//right of leg.
		else
			parts[1].angle = parts[0].angle + 3.14 / 2;
		parts[2].angle = parts[1].angle + 0.1f;
		parts[3].angle = parts[1].angle - 0.1f;
		for(int a=1;a < 4; a++)
		{
			parts[a].start = parts[0].end;
			parts[a].getEnd();
		}
		

	}
	
	void draw(ws::Window &window)
	{
		for(int a=0; a < 4; a++)
			window.draw(parts[a]);
	}
	
};

class PolyPed
{
	public:
	struct LegData
	{
		Leg leg;
		ws::Vec2f normal;
		bool isFar;
	};
	std::vector<LegData> data;
	
	ws::Vec2f scale;
	float rotation = 0;
	float radius = 0;
	ws::Vec2f center = {0,0};
	
	void init(float radius,std::vector<ws::Vec2f> normals)
	{
		this->radius = radius;
		data.reserve(normals.size());
		for(int a=0;a<normals.size();a++)
		{
			Leg leg;
			leg.init(normals[a] * radius);
			data.push_back({leg,normals[a],false});
		}
	}
	
	void setFar(int index)
	{
		if(index < 0 || index >= data.size())
			return;
		data[index].isFar = true;
	}
	void setNear(int index)
	{
		if(index < 0 || index >= data.size())
			return;
		data[index].isFar = false;
	}
	
	void setPosition(ws::Vec2f pos)
	{center = pos;}
	void setRadius(float radius)
	{this->radius = radius;}
	void setRotation(float rot)
	{this->rotation = rot;}
	void setScale(ws::Vec2f scale)
	{this->scale = scale;}
	
	void setPhases(std::vector<float> phases)
	{
		for(int a=0; a < phases.size(); a++)
		{
			if(data.size() < a || a < 0)
				continue;
			data[a].leg.phase = phases[a];
		}
	}
	void setAmplitudes(std::vector<float> amps)
	{
		for(int a=0; a < amps.size(); a++)
		{
			if(data.size() < a || a < 0)
				continue;
			data[a].leg.amplitude = amps[a];
		}		
	}
	
	void update(float phase)
	{
		//transformations
		for(int a=0;a < data.size();a++)
		{
			data[a].leg.faceLeft = (scale.x < 0 || scale.y < 0);
				
			data[a].leg.parts[0].start = center + (data[a].normal * radius) * scale;
			data[a].leg.update(phase);
		}
	}
	
	void drawFar(ws::Window &window)
	{
		for(int a=0;a < data.size();a++)
		{
			if(data[a].isFar)
				data[a].leg.draw(window);
		}
	}
	void drawNear(ws::Window &window)
	{
		for(int a=0;a < data.size();a++)
		{
			if(!data[a].isFar)
				data[a].leg.draw(window);
		}		
	}
	Leg &getLeg(int index)
	{
		if(index < 0 || index >= data.size())
		{
			std::cerr << "Out of bounds attempt to access leg! Crashing!!!" << std::endl;
			system("pause");
			exit(0);
		}
		return data[index].leg;
	}
};

float gravity = 0.15;
ws::Texture body;
ws::Texture face;
class Mello
{
	public:
	PolyPed legs;
	float rate = 1.0f;
	float scale = 1.8;
	ws::Sprite sprite;
	ws::Vec2f velocity = {0,0};
	ws::Sprite sprite2;
	
	enum State
	{
		RUNNING,
		DANGLING,
		WALKING,
		STANDING
	}state,lastState;
	
	
	
	void init(ws::Vec2f center)
	{
		state = RUNNING;
		sprite.setTexture(body);
		sprite.setTextureRect({0,0,60,60});
		sprite.setOrigin(60/2,60/2);
		sprite.setPosition(center);
		sprite.setScale(scale,scale);
		
		sprite2.setTexture(face);
		sprite2.setTextureRect({0,0,20,20});
		sprite2.setOrigin({-2,+11});//set origin to be the center of the body. topleft of face - 2 is how far it has to go before it reaches body center position.
		sprite2.setScale(scale,scale);
		
		legs.init((sprite.getTextureRect().width/2) - 10,{
			{-0.6,0},
			{-0.4,0},
			{0.2,0},
			{0.4,0}
		});
		legs.setFar(1);
		legs.setFar(3);
		
	}
	
	void update()
	{
		static float globalTime = 0.0f;
        globalTime += rate; 
		
		if(std::abs(velocity.x) > 5)
			state = RUNNING;
		if(std::abs(velocity.x) < 5 && state != DANGLING)
			state = WALKING;
		if(std::abs(velocity.x) <= 0.01 && (state == RUNNING || state == WALKING))
		{
			velocity.x = 0;
			state = STANDING;
		}

		if(velocity.x < 0)
		{
			sprite.setScale(-scale,scale);
			legs.setScale({-scale,scale});
		}
		else
		{
			sprite.setScale(scale,scale);
			legs.setScale({scale,scale});
		}
		
		
		if(state == STANDING)
		{
			legs.setPhases({0,0,0,0});
			legs.setAmplitudes({0,0,0,0});					
		}
		
		if(state == DANGLING)
		{
			rate = 0.2;
			legs.setPhases({0,1.57,3.14,4.71});
			legs.setAmplitudes({0.8,0.8,0.8,0.8});
		}
		if(state == WALKING)
		{
			if(velocity.x != 0)
				rate = velocity.x/50;
			legs.setPhases({1.57, 0, 4.71, 3.14});
			legs.setAmplitudes({0.4, 0.8, 0.8, 0.4});
		}
		if(state == RUNNING)
		{
			if(velocity.x != 0)
				rate = velocity.x/50;

			legs.setPhases({0,3.14,3.14,0});
			legs.setAmplitudes({0.8, 1.2, 1.2, 0.8});
		}
		
		float radius = legs.getLeg(0).parts[0].length + 1;
		
		if(sprite.getPosition().y + radius < 540)
		{
			if(sprite.getPosition().y + radius < 540 - 10)
			{
				if(state != DANGLING)
					lastState = state;
				state = DANGLING;
			}
			velocity.y += gravity;
		}
		else
		{
			state = lastState;
			velocity.y = 0;
			sprite.setPosition(sprite.getPosition().x,540-radius);
		}
		
		if(ws::Global::getButton(ws::Key::Left))
			velocity.x -= 0.2;
		if(ws::Global::getButton(ws::Key::Right))
			velocity.x += 0.2;
		if(ws::Global::getButton(ws::Key::Up))
			velocity.y = -5;
		
		
		
		sprite.move(velocity.x,velocity.y);
		sprite2.setPosition(sprite.getPosition());
		sprite2.setScale(sprite.getScale());
		legs.setPosition(sprite.getPosition());
		legs.update(globalTime);
	}
	
	void draw(ws::Window &window)
	{
		legs.drawFar(window);
		window.draw(sprite);
		window.draw(sprite2);
		legs.drawNear(window);
	}
	
};


int main()
{
	body.loadFromFile("body.png");
	body.setScaleMode(ws::Texture::ScaleMode::NearestNeighbor);
	
	face.loadFromFile("faces.png");
	face.setScaleMode(ws::Texture::ScaleMode::NearestNeighbor);
	
	ws::Window window(960,540,"");
	
	Mello mello;
	mello.init({960/2,540/2});
	
	ws::Slider slider;
	slider.setRange(0,500);
	slider.setSize(960,20);
	window.addChild(slider);
	
	ws::Timer timer;
	float time = 0;
	float tpf = 1.f/60.f;
	
	while(window.isOpen())
	{
		float dt = timer.restart();
		time += dt;
		while(time > tpf)
		{
			time -= tpf;
			mello.update();
		}
		
		int legHeight = slider.getSlidePosition();
		
		for(int a=0;a<4;a++)
			mello.legs.getLeg(a).parts[0].length = legHeight;
		
		
		//window.getView().setCenter(mello.sprite.getPosition());
		window.clear(ws::Hue::cyan);
		mello.draw(window);
		window.display();
	}
	return 0;
} 