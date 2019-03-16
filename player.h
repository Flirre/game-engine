

// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 
	int hits = 0;

	virtual ~Player() { SDL_Log("Player::~Player"); }

	virtual void Init()
	{
		SDL_Log("Player::Init");
		GameObject::Init();
		lives = NUM_LIVES;
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			//SDL_Log("Player::TopHit");
		}
		if (m == SIDE_HIT) 
		{
			hits++;
			//SDL_Log("Player::SideHit! %d", hits);
			RemoveLife();

			if (lives < 0)
				Send(GAME_OVER);
		}
		if (m == ON_MAP)
		{
			hits++;
			verticalVelocity = 0;
			onGround = true;
			SetSprite(GetSprites().at(0));
		}
		if (m == NOT_ON_MAP)
		{
			onGround = false;
		}
		if (m == JUMP) 
		{
			SetSprite(GetSprites().at(1));
		}
	}

	void RemoveLife()
	{
		//lives--;
		//SDL_Log("remaining lives %d", lives);
	}
};

class PlayerPhysicsComponent : public Component
{
private: 
	Player * player;

	bool is_walking_left = false;
	bool is_walking_right = false;

	bool space_released = true;

public:
	virtual ~PlayerPhysicsComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);
		player = (Player*) go;
	}

	virtual void Init()
	{
		go->spriteWidth = 16;
		go->spriteHeight = 21;

		go->horizontalPosition = WORLD_WIDTH/2;
		go->verticalPosition = 0;
		player_horizontal_position = go->horizontalPosition;
		go->prevHPos = go->horizontalPosition;
		go->prevVPos = go->verticalPosition;

		go->horizontalVelocity = 0;
		go->verticalVelocity = 0;
		go->prevHVel = 0;
		go->prevVVel = 0;
	}

	void CheckBounds() {
		if (go->horizontalPosition > (WORLD_WIDTH - go->spriteWidth))
			go->horizontalPosition = 0;

		if (go->horizontalPosition < 0)
			go->horizontalPosition = WORLD_WIDTH - go->spriteWidth;

		// Ground
		if (go->verticalPosition > GROUND_POSITION)
			go->verticalPosition = GROUND_POSITION;
	}

	virtual void UpdatePosition(float dt)
	{
		go->prevHPos = go->horizontalPosition;
		go->prevVPos = go->verticalPosition;
		go->prevHVel = go->horizontalVelocity;
		go->prevVVel = go->verticalVelocity;

		go->horizontalPosition += go->horizontalVelocity * dt;
		player_horizontal_position = go->horizontalPosition;
		go->verticalPosition += go->verticalVelocity	 * dt;
		// If above the ground, apply gravity
		if (go->verticalPosition < GROUND_POSITION) {
			go->verticalVelocity -= GRAVITY * dt;
		}
	}

	virtual void Update(float dt)
	{
		UpdatePosition(dt);
		CheckBounds();
	}

};

class PlayerInputComponent : public Component
{
private: Player * player;

		 bool is_walking_left = false;
		 bool is_walking_right = false;

		 bool space_released = true;

public: 
	virtual ~PlayerInputComponent() {}
	
	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);
		player = (Player*)go;
	}

	virtual void Init()
	{
		// Spawn facing right
		go->direction = RIGHT;
	}

	void WalkLeft()
	{
		is_walking_left = true;
		go->direction = LEFT;

		if (abs(go->horizontalVelocity) < PLAYER_SPEED)
		{
			go->horizontalVelocity = -PLAYER_SPEED;
		}
	}

	void WalkRight()
	{
		is_walking_right = true;
		go->direction = RIGHT;

		if (abs(go->horizontalVelocity) < PLAYER_SPEED)
		{
			go->horizontalVelocity = PLAYER_SPEED;
		}
	}

	void Jump()
	{
		if (go->onGround == true)
		{
			go->verticalVelocity = -250.0f;
			go->onGround = false;
			go->Receive(JUMP);
		}
	}

	void Stop()
	{
		go->horizontalVelocity = 0;
		is_walking_left = false;
		is_walking_right = false;
	}

	void UpdateMovement(AvancezLib::KeyStatus keys)
	{
		if (keys.left) {
			WalkLeft();
		}

		if (keys.right) {
			WalkRight();
		}

		if ((is_walking_right && keys.right == false) || (is_walking_left && keys.left == false)) {
			Stop();
		}

		if (keys.space && space_released) {
			space_released = false;
			Jump();
		}

		if (!keys.space) {
			space_released = true;
		}
	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);
		UpdateMovement(keys);
	}
};
class PlayerBehaviourComponent : public Component
{

private:
	Player * player;

	bool is_walking_left = false;
	bool is_walking_right = false;

	bool space_released = true;

	Sprite * idle_sprite;
	Sprite * jump_sprite;

public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);
		player = (Player*) go;
	}

	virtual void Init()
	{
	}

	void Jump() 
	{
		//if (go->onGround == true) {
		//	go->verticalVelocity = -250.0f;
		//	go->onGround = false;
		//	go->SetSprite(jump_sprite);
		//}
	}



	// Update the player
	// physics depend on the time.
	virtual void Update(float dt)
	{

	}

};
