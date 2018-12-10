

// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 

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
			SDL_Log("Player::Hit!");
			RemoveLife();

			if (lives < 0)
				Send(GAME_OVER);
		}
	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};


class PlayerBehaviourComponent : public Component
{

private:
	Player * player;

	bool is_walking_left = false;
	bool is_walking_right = false;

	bool space_released = true;

	Sprite * sprite;

public:
	virtual ~PlayerBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);

		player = (Player*) go;

		sprite = system->createSprite("data/bmps/frame6.bmp");


	}

	virtual void Init()
	{
		go->horizontalPosition = WORLD_WIDTH/2;
		go->verticalPosition = WORLD_HEIGHT - 21; // 21 = Mario sprite height

		
		player_horizontal_position = go->horizontalPosition;

		go->horizontalVelocity = 0;
		go->verticalVelocity = 0;

		// Spawn facing right
		go->direction = RIGHT;

	}


	// Basic euler integration
	void UpdatePhysics(float dt) {
		go->horizontalPosition += go->horizontalVelocity * dt;
		player_horizontal_position = go->horizontalPosition;
		go->verticalPosition += go->verticalVelocity	 * dt;

		// If above the ground, apply gravity
		if (go->verticalPosition < GROUND_POSITION) {
			go->verticalVelocity -= GRAVITY * dt;
		}
	}

	void CheckBounds() {
		if (go->horizontalPosition > (WORLD_WIDTH - PLAYER_SIZE))
			go->horizontalPosition = WORLD_WIDTH - PLAYER_SIZE;

		if (go->horizontalPosition < 0)
			go->horizontalPosition = 0;

		// Ground
		if (go->verticalPosition > GROUND_POSITION)
			go->verticalPosition = GROUND_POSITION;
	}

	// Apply movement based on input keys
	void UpdateMovement(AvancezLib::KeyStatus keys) {
		if (keys.left) {
			WalkLeft();
		}

		if (is_walking_left && keys.left == false) {
			is_walking_left = false;
			Stop();
		}

		if (keys.right) {
			WalkRight();
		}

		if (is_walking_right && keys.right == false) {
			is_walking_right = false;
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

	void WalkLeft() {
		// Change sprite first time and send event to AI
		if (!is_walking_left) {
			go->SetSprite(sprite);
		}
		is_walking_left = true;

		go->direction = LEFT;

		if (abs(go->horizontalVelocity) < PLAYER_SPEED) {
			go->horizontalVelocity = -PLAYER_SPEED;
		}

	}

	void WalkRight() {
		// Change sprite first time
		if (!is_walking_right) {
			go->SetSprite(sprite);
		}
		is_walking_right = true;

		go->direction = RIGHT;

		if (abs(go->horizontalVelocity) < PLAYER_SPEED) {
			go->horizontalVelocity = PLAYER_SPEED;
		}
	}

	void Jump() {
		if (go->verticalPosition >= GROUND_POSITION) {
			go->verticalVelocity = -250.0f;
		}
	}

	void Stop() {
		go->horizontalVelocity = 0;
	}


	// Update the palyer
	// physics depend on the time.
	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);

		UpdatePhysics(dt);
		UpdateMovement(keys);
		CheckBounds();
	}

};
