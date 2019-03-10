#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include "windows.h"
#include "winnt.h"

void Component::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->system = system;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * system, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(system, go, game_objects);

	sprite = system->createSprite(sprite_name);
}

void RenderComponent::SetSprite(Sprite * s) {
	sprite = s;
}


void RenderComponent::Update(float dt)
{
	if (!go->enabled)
		return;

	if (sprite)
		sprite->draw(int(go->horizontalPosition), int(go->verticalPosition), go->direction);
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* system, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(system, go, game_objects);
	this->coll_objects = coll_objects;
}

bool boundingBoxCollision(GameObject* go, GameObject* go0)
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
		SDL_Log("Left");
		correctedLocation.first = go0->horizontalPosition - go->spriteWidth - 1;
		break;
	case Right:
		SDL_Log("Right");
		correctedLocation.first = go0->horizontalPosition + go0->spriteWidth + 1;
		break;
	case Top:
		SDL_Log("Top");
		correctedLocation.second = go0->verticalPosition - go->spriteHeight - 1;
		go->Receive(ON_MAP);
		break;
	case Bottom:
		SDL_Log("Bottom");
		correctedLocation.second = go0->verticalPosition + go0->spriteHeight + 1;
		go->verticalVelocity = 0;
	default:
		break;
	}
	return correctedLocation;
}

void ResolveCollision(GameObject* go, GameObject* go0, float dt)
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
					SDL_Log("GAMO::VertPos=%d", go->verticalPosition);
					SDL_Log("GAMO MAPO DIFF=%d", (go0->verticalPosition - go->verticalPosition));
					go->Receive(HIT);
					go0->Receive(HIT);
				}
			}
		}
	}
}
