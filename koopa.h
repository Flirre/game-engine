#pragma once

// a koopa
class Koopa : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 

	virtual ~Koopa() { SDL_Log("Koopa::~Koopa"); }

	virtual void Init()
	{
		SDL_Log("Koopa::Init");
		GameObject::Init();
		lives = 100;
	}

	virtual void Receive(Message m)
	{
		if (m == HIT)
		{
			SDL_Log("Koopa::Hit!");
			RemoveLife();
			if (lives < 0) {
				SDL_Log("Koopa:Ded!");
				this->enabled = false;
			}
		}
		if (m == SIDE_HIT)
		{
			SDL_Log("Koopa::SIDESLAMMER");
		}

	}

	void RemoveLife()
	{
		lives--;
		SDL_Log("remaining lives %d", lives);
	}
};


class KoopaBehaviourComponent : public Component
{

private:
	Koopa * koopa;

	bool is_walking_left = false;
	bool is_walking_right = false;
	bool space_released = true;
	float koopa_horizontal_position = 0.0f;
	const float KOOPA_SPEED = 60.0f;

	Sprite * sprite;

public:
	virtual ~KoopaBehaviourComponent() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);

		koopa = (Koopa*) go;
		go->direction = RIGHT;
		//sprite = system->createSprite("data/bmps/frame38.bmp");


	}

	virtual void Init()
	{
		go->spriteWidth = 16;
		go->spriteHeight = 16;
		go->horizontalPosition = WORLD_WIDTH / 3;
		go->verticalPosition = WORLD_HEIGHT/2;


		koopa_horizontal_position = go->horizontalPosition;

		go->horizontalVelocity = 0;
		go->verticalVelocity = 0;

		// Spawn facing right


	}


	// Basic euler integration
	void UpdatePhysics(float dt) {
		go->horizontalPosition += go->horizontalVelocity * dt;
		koopa_horizontal_position = go->horizontalPosition;
		go->verticalPosition += go->verticalVelocity	 * dt;

		// If above the ground, apply gravity
		if (go->verticalPosition < GROUND_POSITION) {
			go->verticalVelocity -= GRAVITY * dt;
		}
	}

	void CheckBounds() {
		if (go->horizontalPosition > (WORLD_WIDTH - go->spriteWidth))
			go->horizontalPosition = 0;

		if (go->horizontalPosition < 0)
			go->horizontalPosition = WORLD_WIDTH - go->spriteWidth;

		// Ground
		if (go->verticalPosition > GROUND_POSITION+5)
			go->verticalPosition = GROUND_POSITION+5;
	}

	// Apply movement based on input keys
	void UpdateMovement(AvancezLib::KeyStatus keys) {
		if (go->direction == LEFT) {
			WalkLeft();
		}

		if (go->direction == RIGHT) {
			WalkRight();
		}

		if (keys.space && space_released) {
			//ChangeDirection();
			space_released = false;
			//Bounce();
		}

		if (!keys.space) {
			space_released = true;
		}
	}


	void WalkLeft() {
		// Change sprite first time and send event to AI
		if (!is_walking_left) {
			//go->SetSprite(sprite);
		}
		is_walking_left = true;

		go->direction = LEFT;

		if (abs(go->horizontalVelocity) < KOOPA_SPEED) {
			go->horizontalVelocity = -KOOPA_SPEED;
		}

	}

	void WalkRight() {
		// Change sprite first time
		if (!is_walking_right) {
			//go->SetSprite(sprite);
		}
		is_walking_right = true;

		go->direction = RIGHT;

		if (abs(go->horizontalVelocity) < KOOPA_SPEED) {
			go->horizontalVelocity = KOOPA_SPEED;
		}
	}

	void Bounce() {
		if (true) {
			go->verticalVelocity = -125.0f;
		}
	}

	void Stop() {
		go->horizontalVelocity = 0;
		is_walking_left = false;
		is_walking_right = false;
	}

	void ChangeDirection() {
		Stop();
		if (go->direction == RIGHT) {
			WalkLeft();
			return;
		}
		if (go->direction == LEFT) {
			WalkRight();
			return;
		}
		return;
	}


	// Update the player
	// physics depend on the time.
	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);

		UpdatePhysics(dt);
		//UpdateMovement(keys);
		CheckBounds();
	}

};
