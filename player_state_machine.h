#pragma once
// based on player_state_machine from lab5 - plebfighters
class PlayerStateMachine : public Component
{
	// The state interface
	class State
	{
	public:
		AvancezLib * system;
		Sprite * sprite;
	public:
		virtual ~State() {}
		virtual void Enter(PlayerStateMachine& state_machine) {}
		virtual void HandleInput(PlayerStateMachine& state_machine, AvancezLib::KeyStatus input) {}
		virtual void Update(PlayerStateMachine& state_machine) {}
	};

	class IdleState : public State
	{
	private:

		bool attack_released = true;

	public:
		IdleState(AvancezLib* system) {
			this->system = system;
			sprite = system->createSprite("data/bmps/frame6.bmp");
		}

		virtual void Enter(PlayerStateMachine &state_machine) {
			state_machine.current_state = this;
			state_machine.player->SetSprite(sprite);
			if (state_machine.player->direction == RIGHT) {
				state_machine.player->horizontalVelocity = 0.0f;
			}
			if (state_machine.player->direction == LEFT) {
				state_machine.player->horizontalVelocity = 0.0f;
			}
		}

		virtual void HandleInput(PlayerStateMachine &state_machine, AvancezLib::KeyStatus input) {

			/**** Jump transition (press spacebar) ****/
			if (input.space == true && (system->getElapsedTime() > (state_machine.last_attack_time + state_machine.ATTACK_COOLDOWN_TIME)) && attack_released) {
				attack_released = false;	// Toggling
				state_machine.state_attack->Enter(state_machine);
			}
		}

		virtual void Update(PlayerStateMachine &state_machine) {/* Empty update */ }
	};

	class AttackState : State
	{
	private:
		const float ATTACK_SPEED = 100.0f;
	public:
		AttackState(AvancezLib* system) {
			this->system = system;
			sprite = system->createSprite("data/bmps/frame6.bmp");
		}

		virtual void Enter(PlayerStateMachine &state_machine) {
			state_machine.current_state = this;
			state_machine.last_attack_time = system->getElapsedTime();
			state_machine.player->SetSprite(sprite);
			if (state_machine.player->direction == RIGHT) {
				state_machine.player->horizontalVelocity += ATTACK_SPEED;
			}
			if (state_machine.player->direction == LEFT) {
				state_machine.player->horizontalVelocity += -ATTACK_SPEED;
			}
		}

		virtual void HandleInput(PlayerStateMachine &state_machine, AvancezLib::KeyStatus input) {
			// No input while attacking
		}

		virtual void Update(PlayerStateMachine &state_machine) {
			// Change state to idle when ATTACK is done
			if (system->getElapsedTime() > (state_machine.last_attack_time + state_machine.ATTACK_TIME)) {
				state_machine.state_idle->Enter(state_machine);
			}
		}
	};


public:
	float last_attack_time = 0.0f;

	const float	DASH_TIME = 0.2f;
	const float	DASH_COOLDOWN_TIME = 0.8f;

	const float	ATTACK_TIME = 0.05f;
	const float	ATTACK_COOLDOWN_TIME = 0.05f;

	const float	CROUCH_PUNCH_TIME = 0.2f;
	const float	CROUCH_PUNCH_COOLDOWN_TIME = 0.3f;

	const float	CROUCH_KICK_TIME = 0.3f;
	const float	CROUCH_KICK_COOLDOWN_TIME = 0.4f;

	Player		* player;
	AvancezLib  * system;

	State *	current_state;

	IdleState *			state_idle;
	AttackState *		state_attack;


	virtual ~PlayerStateMachine() {}

	virtual void Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects)
	{
		Component::Create(system, go, game_objects);

		this->system = system;
		player = (Player*)go;
	}

	virtual void Init()
	{
		state_idle = new IdleState(system);
		state_attack = new AttackState(system);


		current_state = state_idle;
	}

	// Main loop
	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		system->getKeyStatus(keys);
		current_state->HandleInput(*this, keys);
		current_state->Update(*this);
	}
};

