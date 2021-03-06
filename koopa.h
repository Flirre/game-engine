#pragma once

class KoopaInputComponent : public InputComponent
{
public:
	void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, Direction direction)
	{
		InputComponent::Create(system, go, game_objects, KOOPA_SPEED, direction);
		go->direction = direction;
	}

	virtual void Receive(Message m)
	{
		if (m == IDLE)
		{
			Stop();
		}

	}

	void Turn_Around()
	{
		Stop();
		if (go->direction == RIGHT)
		{
			WalkLeft();
		}
		else
		{
			WalkRight();
		}
	}

	void Update(float dt)
	{
		if (go->direction == RIGHT)
		{
			WalkRight();
		}
		else
		{
			WalkLeft();
		}
	}
};

class KoopaCollideComponent : public CollideComponent
{
public:
	void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects) 
	{
		CollideComponent::Create(system, go, game_objects, coll_objects);
	}

	void Update(float dt)
	{
		{
			for (auto i = 0; i < coll_objects->pool.size(); i++)
			{
				GameObject * go0 = coll_objects->pool[i];
				if (go0->enabled && go != go0) {
					if (boundingBoxCollision(go, go0)) // check for any AABB collision
					{
							ResolveCollision(go, go0, dt);
							if (!go0->map_object && (abs(go->verticalPosition - go0->verticalPosition) < 2))
							{
								go->Receive(TURN_AROUND);
								go0->Receive(TURN_AROUND);
							}
					}
				}
			}
		}
	}

};

class KoopaPhysicsComponent : public PhysicsComponent 
{
public:
	void CheckBounds(unsigned int WORLD_WIDTH, GameObject* go) {
		if (go->horizontalPosition > (WORLD_WIDTH - go->spriteWidth / 2))
			go->horizontalPosition = 0;

		if (go->horizontalPosition < -(go->spriteWidth / 2))
			go->horizontalPosition = WORLD_WIDTH - go->spriteWidth / 2;

		// check if koopa is at same height as pipe
		if (go->verticalPosition > 190) 
		{
			// teleport to corresponding top pipe
			if (go->horizontalPosition < 16) 
			{
				go->horizontalPosition = 32;
				go->verticalPosition = 10;
				go->Receive(TURN_AROUND);
			}

			if (go->horizontalPosition > (WORLD_WIDTH - 32))
			{
				go->horizontalPosition = (WORLD_WIDTH - 48);
				go->verticalPosition = 10;
				go->Receive(TURN_AROUND);
			}
		}
	}

	void Update(float dt)
	{
		PhysicsComponent::UpdatePosition(dt, go, this->GetGravity());
		CheckBounds(this->GetWorldWidth(), go);
	}

};

class Koopa : public GameObject
{
public:

	int lives;

	virtual ~Koopa() { SDL_Log("Koopa::~Koopa"); }

	virtual void Init()
	{
		SDL_Log("Koopa::Init");
		GameObject::Init();
		lives = 3;
	}

	virtual void Receive(Message m)
	{
		onGround = false;
		if (m == HIT)
		{
			verticalVelocity = -100.f;
			RemoveLife();
			if (lives < 0) {
				Send(HIT);
				this->enabled = false;
			}
		}
		if (m == ON_MAP)
		{
			verticalVelocity = 0;
			onGround = true;
			SetSprites(GetSpriteSet(0));
		}
		if (m == NOT_ON_MAP)
		{
			onGround = false;
		}
		if (m == JUMP)
		{
			SetSprites(GetSpriteSet(1));
		}
		if (m == RUNNING)
		{
			if (onGround)
			{
				SetSprites(GetSpriteSet(2));
			}
		}
		if (m == IDLE)
		{
			SetSprites(GetSpriteSet(0));
		}
		if (m == TURN_AROUND)
		{
			KoopaInputComponent *koopa_input = (KoopaInputComponent*)(InputComponent*)this->components.at(0); //InputComponent is added at place 0 in the list of Components.
			koopa_input->Turn_Around();
		}

	}

	void RemoveLife()
	{
		lives--;
		//SDL_Log("remaining lives %d", lives);
	}
};
