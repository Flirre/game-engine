#pragma once

// map object
class MapObject : public GameObject
{
public:
	virtual ~MapObject() { SDL_Log("MapObject::MapObject"); }
	bool spawn = false;
	bool active;

	virtual void Init() 
	{
		if (!spawn) {
			SDL_Log("MapObject::Init");
			GameObject::Init();

			this->spriteWidth = 33;
			this->spriteHeight = 9;

			this->horizontalVelocity = 0;
			this->verticalVelocity = 0;

			this->direction = RIGHT;
			this->enabled = true;
			this->map_object = true;
			this->active = false;
		}
	}

	virtual void Init(int width, int height)
	{
			SDL_Log("MapObject::Init");
			GameObject::Init();
			SDL_Log("help");
			this->spriteWidth = width;
			this->spriteHeight = height;

			this->horizontalVelocity = 0;
			this->verticalVelocity = 0;

			this->direction = RIGHT;
			this->enabled = true;
			this->map_object = true;
			this->spawn = true;
	}

	virtual void Receive(Message m)
	{
		if (m == SIDE_HIT)

		{
			if (spawn)
			{
				SDL_Log("SIDE TRUE");
				enabled = true;
			}
		}

		if (m == SIDE_HIT)
		{
			if (spawn)
			{
				SDL_Log("JUMP FALSE");
				enabled = true;
			}
		}

		if (m == BOUNCE)
		{
			if (!spawn)
			{
				SDL_Log("imactive");
				active = true;
				SetSprites(GetSpriteSet(1));
			}
		}
	}
};