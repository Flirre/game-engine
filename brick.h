#pragma once

// brick object
class Brick : public MapObject
{
public:
	virtual ~Brick() { SDL_Log("Brick::Brick"); }

	virtual void Init()
	{
		SDL_Log("Bricks::Init");
		MapObject::Init();

		this->spriteWidth = 16;
		this->spriteHeight = 15;

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