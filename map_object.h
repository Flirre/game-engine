#pragma once

// map object
class MapObject : public GameObject
{
public:
	virtual ~MapObject() { SDL_Log("MapObject::MapObject"); }

	virtual void Init() 
	{
		SDL_Log("MapObject::Init");
		GameObject::Init();

		this->spriteWidth = 33;
		this->spriteHeight = 9;

		this->horizontalVelocity = 0;
		this->verticalVelocity = 0;

		this->direction = RIGHT;
		this->enabled = true;
		this->map_object = true;
	}

	virtual void Receive(Message m)
	{

	}
};