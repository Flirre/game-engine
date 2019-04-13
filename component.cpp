#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include "windows.h"
#include "winnt.h"
class AvancezLib;

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, std::vector<std::vector<Sprite*>> sprites)
{
	Component::Create(system, go, game_objects);
	this->sprites = sprites;
	spriteSet = sprites.at(0);
}

void RenderComponent::SetSprites(std::vector<Sprite *> s) {
	spriteSet = s;
}


void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;
	sprite = spriteSet.at((SDL_GetTicks() / 50) % spriteSet.size());
	if (sprite)
		sprite->draw(int(go->horizontalPosition), int(go->verticalPosition), go->direction);
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}

void PhysicsComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, float horizontalPosition, float verticalPosition, int spriteWidth, int spriteHeight, unsigned int WORLD_WIDTH, unsigned int WORLD_HEIGHT, float GRAVITY, float speed)
{
	Component::Create(system, go, game_objects);
	this->WORLD_WIDTH = WORLD_WIDTH;
	this->WORLD_HEIGHT = WORLD_HEIGHT;
	this->GRAVITY = GRAVITY;
	this->SPEED = speed;

	go->spriteWidth = spriteWidth;
	go->spriteHeight = spriteHeight;

	go->horizontalPosition = horizontalPosition;
	go->verticalPosition = verticalPosition;

	go->prevHPos = go->horizontalPosition;
	go->prevVPos = go->verticalPosition;

	go->horizontalVelocity = 0;
	go->verticalVelocity = 0;
	go->prevHVel = 0;
	go->prevVVel = 0;
	

}

void PhysicsComponent::CheckBounds(unsigned int WORLD_WIDTH, GameObject* go) {
	if (go->horizontalPosition > (WORLD_WIDTH - go->spriteWidth / 2))
		go->horizontalPosition = 0;

	if (go->horizontalPosition < -(go->spriteWidth / 2))
		go->horizontalPosition = WORLD_WIDTH - go->spriteWidth / 2;
}

void PhysicsComponent::UpdatePosition(float dt, GameObject* go, float GRAVITY)
{
	go->prevHPos = go->horizontalPosition;
	go->prevVPos = go->verticalPosition;
	go->prevHVel = go->horizontalVelocity;
	go->prevVVel = go->verticalVelocity;

	go->horizontalPosition += go->horizontalVelocity * dt;
	go->verticalPosition += go->verticalVelocity	 * dt;
	
	// always apply gravity
	go->verticalVelocity -= GRAVITY * dt;
}

void PhysicsComponent::Update(float dt)
{
	UpdatePosition(dt, go, GRAVITY);
	CheckBounds(WORLD_WIDTH, go);
}

void InputComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, float SPEED, Direction direction)
{
	Component::Create(system, go, game_objects);
	go->direction = direction;
	is_walking_left = false;
	is_walking_right = false;
	is_jumping = false;
	space_released = true;
	goSPEED = SPEED;
}

void InputComponent::WalkLeft()
{
	go->Receive(RUNNING);
	is_walking_left = true;
	go->direction = LEFT;

	if (abs(go->horizontalVelocity) < goSPEED)
	{
		go->horizontalVelocity = -goSPEED;
	}
}

void InputComponent::WalkRight()
{
	go->Receive(RUNNING);
	is_walking_right = true;
	go->direction = RIGHT;

	if (abs(go->horizontalVelocity) < goSPEED)
	{
		go->horizontalVelocity = goSPEED;
	}
}

void InputComponent::Jump()
{
	if (go->onGround == true)
	{
		go->verticalVelocity = -250.0f;
		go->onGround = false;
		is_jumping = true;
		go->Receive(JUMP);
	}
}

void InputComponent::UpdateMovement(AvancezLib::KeyStatus keys)
{
	if (!is_jumping)
	{
		if (keys.left && !is_walking_right) {
			WalkLeft();
		}

		if (keys.right && !is_walking_left) {
			WalkRight();
		}

		if ((is_walking_right && keys.right == false) || (is_walking_left && keys.left == false)) {
			Stop();
		}
	}
	if (keys.space && space_released) {
		space_released = false;
		Jump();
	}

	if (!keys.space) {
		space_released = true;
	}
}

void InputComponent::Stop()
{
	go->horizontalVelocity = 0;
	is_walking_left = false;
	is_walking_right = false;
	if (go->onGround)
	{
	go->Receive(IDLE);
	}
}

void InputComponent::Update(float dt)
{
	AvancezLib::KeyStatus keys;
	system->getKeyStatus(keys);
	UpdateMovement(keys);

	if (go->onGround)
	{
		is_jumping = false;
	}
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}

bool CollideComponent::boundingBoxCollision(GameObject* go, GameObject* go0)
{	

	double goLeftEdge = go->horizontalPosition;
	double goRightEdge = go->horizontalPosition + go->spriteWidth;
	double goBottomEdge = go->verticalPosition + go->spriteHeight;
	double goTopEdge = go->verticalPosition;

	double go0LeftEdge = go0->horizontalPosition;
	double go0RightEdge = go0->horizontalPosition + go0->spriteWidth;
	double go0BottomEdge = go0->verticalPosition + go0->spriteHeight;
	double go0TopEdge = go0->verticalPosition;

	bool go_Right_Of_go0 = (goLeftEdge > go0RightEdge) ? true : false;
	bool go_Left_Of_go0 = (goRightEdge < go0LeftEdge) ? true : false;
	bool go_Above_go0 = (goBottomEdge < go0TopEdge) ? true : false;
	bool go_Below_go0 = (goTopEdge > go0BottomEdge) ? true : false;
	bool colliding = (!(go_Right_Of_go0 || go_Left_Of_go0 || go_Above_go0 || go_Below_go0)) ? true : false;
	return (colliding);
}

enum CollisionSide
{
	None = 0,
	Top = 1,
	Bottom = 2,
	Left = 4,
	Right = 8
};
DEFINE_ENUM_FLAG_OPERATORS(CollisionSide)

CollisionSide GetCollisionSideFromSlopeComparison(CollisionSide potentialSides, double velocitySlope, double nearestCornerSlope)
{
	if ((potentialSides & Top) == Top)
	{
		if ((potentialSides & Left) == Left)
			return velocitySlope < nearestCornerSlope ? Top : Left;
		if ((potentialSides & Right) == Right)
			return velocitySlope > nearestCornerSlope ? Top : Right;
	}
	if ((potentialSides & Bottom) == Bottom)
	{
		if ((potentialSides & Left) == Left)
			return velocitySlope > nearestCornerSlope ? Bottom : Left;
		if ((potentialSides & Right) == Right)
			return velocitySlope < nearestCornerSlope ? Bottom : Right;
	}
	// method should never end up here.
	SDL_Log("--------------------------------------------------------\n\n\n\n\n\n\n------------------------------------------------NONE");
	return None;
}

CollisionSide GetCollisionSide(GameObject* go, GameObject* go0, float dt) 
{
	double goLeft = go->prevHPos;
	double goRight = go->prevHPos + go->spriteWidth;
	double goBottom = go->prevVPos + go->spriteHeight;
	double goTop = go->prevVPos;
	double goXVel = go->horizontalVelocity;
	double goYVel = go->verticalVelocity;

	double go0Left = go0->horizontalPosition;
	double go0Right = go0->horizontalPosition + go0->spriteWidth;
	double go0Bottom = go0->verticalPosition + go0->spriteHeight;
	double go0Top = go0->verticalPosition;
	
	double cornerSlopeRise = 0;
	double cornerSlopeRun = 0;

	double velocitySlope = goYVel / goXVel;

	CollisionSide potentialCollisionSide = None;
	if (goRight <= go0Left)
	{
		potentialCollisionSide |= Left;

		cornerSlopeRun = go0Left - goRight;

		if (goBottom <= go0Top)
		{
			potentialCollisionSide |= Top;
			cornerSlopeRise = go0Top - goBottom;
		}
		if (goTop >= go0Bottom)
		{
			potentialCollisionSide |= Bottom;
			cornerSlopeRise = go0Bottom - goTop;
		}
		else
		{
			return Left;
		}
	}
	if (goLeft >= go0Right)
	{
		potentialCollisionSide |= Right;
		cornerSlopeRun = goLeft - go0Right;

		if (goBottom <= go0Top)
		{
			potentialCollisionSide |= Top;
			cornerSlopeRise = goBottom - go0Top;
		}
		if (goTop >= go0Bottom)
		{
			potentialCollisionSide |= Bottom;
			cornerSlopeRise = goTop - go0Bottom;
		}
		else
		{
			return Right;
		}
	}
	else 
	{
		// Did not collide on either left or right side.
		if (goBottom <= go0Top)
		{
			return Top;
		}
		if (goTop >= go0Bottom)
		{
			return Bottom;
		}
		else
		{
			// None is returned because go was already colliding with go0.
			return None;
		}
	}
	////Corner case; might have collided with more than one side
	//Compare slopes to see which side was collided with
	return GetCollisionSideFromSlopeComparison(potentialCollisionSide, velocitySlope, cornerSlopeRise / cornerSlopeRun);
}



std::pair<double, double> GetCorrectedLocation(GameObject* go, GameObject* go0, CollisionSide collisionSide)
{
	std::pair<double, double> correctedLocation = std::make_pair(go->horizontalPosition, go->verticalPosition);
	switch (collisionSide)
	{
	case Left:
		correctedLocation.first = go0->horizontalPosition - go->spriteWidth - 1;
		go->Receive(NOT_ON_MAP);
		break;
	case Right:
		correctedLocation.first = go0->horizontalPosition + go0->spriteWidth + 1;
		go->Receive(NOT_ON_MAP);
		break;
	case Top:
		correctedLocation.second = go0->verticalPosition - go->spriteHeight - 1;
		go->Receive(ON_MAP);
		break;
	case Bottom:
		correctedLocation.second = go0->verticalPosition + go0->spriteHeight + 1;
		go->Receive(NOT_ON_MAP);
		go->verticalVelocity = 0;
	default:
		break;
	}
	return correctedLocation;
}

void CollideComponent::ResolveCollision(GameObject* go, GameObject* go0, float dt)
{
	std::pair < double, double> newLocation = GetCorrectedLocation(go, go0, GetCollisionSide(go, go0, dt));
	go->horizontalPosition = newLocation.first;
	go->verticalPosition = newLocation.second;
}



void CollideComponent::Update(float dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled) {
			if (boundingBoxCollision(go, go0)) // check for any AABB collision
			{
				if (go0->map_object)
				{
					ResolveCollision(go, go0, dt);
				}
				else
				{
					go->Receive(HIT);
				}
			}
		}
	}
}
