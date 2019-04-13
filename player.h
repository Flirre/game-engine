

// the main player
class Player : public GameObject
{
public:

	int lives;	// it's game over when goes below zero 
	int hits = 0;
	bool alive = true;

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
			lives--;
			Fall();
			SetSprites(GetSpriteSet(3));
			if (lives < 0)
				Send(GAME_OVER);
		}
		if (m == ON_MAP)
		{
			hits++;
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
	}

	void Respawn()
	{
		alive = true;
		Receive(IDLE);
		horizontalPosition = WORLD_WIDTH / 2 - 6;
		verticalPosition = 0 - spriteHeight;
		verticalVelocity = 0;
		SDL_Log("Respawned");
		Send(SPAWN);
	}

	void Fall()
	{
		alive = false;
		horizontalVelocity = 0;
		verticalVelocity = -120;

	}

	void Update(float dt)
	{
		// if player is alive update as usual
		if (alive)
		{
			GameObject::Update(dt);
		}
		// if player is dead, only update some components so sprite can fall through map.
		else
		{
			if (verticalPosition < (WORLD_HEIGHT)) {
				components.at(0)->Update(dt); // physics component (so Mario falls off map)
				components.at(4)->Update(dt); // render component (so Mario is rendered while falling)
			}
			else
			{
				Respawn();
			}
		}
	}
};