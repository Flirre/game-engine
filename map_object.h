#pragma once

// map object
class MapObject : public GameObject
{
public:
	virtual ~MapObject() { SDL_Log("MapObject::MapObject"); }
	bool spawn = false;
	bool active;
	float bounce_time = -100000.f;
	AvancezLib * system;

	virtual void Create(AvancezLib * system)
	{
		this->system = system;
		SDL_Log("MapObject::Create");
		GameObject::Create();
	}

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

	virtual void Update(float dt)
	{
		if (((system->getElapsedTime() - bounce_time) > .2f) && active)
		{
			SetSprites(GetSpriteSet(0));
			active = false;
		}
		else {
		GameObject::Update(dt);
		}
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
			if (!spawn && !active)
			{
				SDL_Log("imactive");
				active = true;
				SetSprites(GetSpriteSet(1));
				bounce_time = system->getElapsedTime();
			}
		}
	}
};