



class Alien : public GameObject
{

public:
	int direction;
	bool * change_direction;

	virtual void Init(bool * change_direction, double xPos, double yPos)
	{
		SDL_Log("Alien::Init");
		this->horizontalPosition = xPos;
		this->verticalPosition = yPos;
		this->change_direction = change_direction;

		direction = 1;

		enabled = true;
	}

	virtual void Receive(Message m)
	{
		if (!enabled)
			return;

		if (m == HIT)
		{
			enabled = false;
			Send(ALIEN_HIT); // re-broadcast the message to signal that the aliens has been hit (used to increase the score)
			SDL_Log("Alien::Hit");
		}
	}

	void ChangeDirection()
	{
		direction *= -1;
		verticalPosition += 32;

		if (verticalPosition > (480 - 32))
			Send(GAME_OVER);
	}

};



class AlienBehaviourComponent : public Component
{
public:
	virtual ~AlienBehaviourComponent() {}

	virtual void Update(float dt)
	{
		Alien * alien = (Alien *)go;

		alien->horizontalPosition += alien->direction * ALIEN_SPEED * dt; // direction * speed * time

		if ((alien->direction == 1) && (alien->horizontalPosition > (640 - 32)))
			*(alien->change_direction) = true;

		if ((alien->direction == -1) && (alien->horizontalPosition < 0))
			*(alien->change_direction) = true;
	}
};