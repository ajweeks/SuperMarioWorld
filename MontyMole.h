#pragma once

#include "Enemy.h"

struct INT2;

/*

	Monty Moles spawn from dirt patches in walls and in the ground when the player comes nearby.
	If the player doesn't kill them, they will respawn when the player goes away and comes back.
	Once a mole has been spawned and has jumped out, whether or not they were killed, there will
	be a hole sprite rendered there for the rest of the game.

	All moles are spawned at the start of the game and are invisible until
	the player comes within a certain distance of them. When a mole is killed by 
	the player, this instance is tol to not spawn a mole again. 

*/

class MontyMole : public Enemy
{
public:
	enum class ANIMATION_STATE
	{
		INVISIBLE, IN_GROUND, JUMPING_OUT_OF_GROUND, WALKING, DEAD
	};
	enum class SPAWN_LOCATION_TYPE
	{
		GROUND, WALL, NONE
	};
	/*
		There are two types of AI:
	  1) Stupid roomba
	    - When these moles spawn, they will see which direction the player is and start walking
		  towards them. They jump every half second or so, and walk until they hit an obstacle,
		  and then turn around.
	  2) Less-stupid rooma
	    - When these moles spawn they also first go in the direction that the player is, but once 
		  they walk past the player, they turn around and walk back, just to turn around soon after.
	*/
	enum class AI_TYPE
	{
		DUMB, SMART, NONE
	};

	MontyMole(DOUBLE2& startingPos, Level* levelPtr, SPAWN_LOCATION_TYPE spawnLocationType, AI_TYPE aiType);
	virtual ~MontyMole();

	MontyMole(const MontyMole&) = delete;
	MontyMole&operator=(const MontyMole&) = delete;

	void Tick(double deltaTime);
	void Paint();
	INT2 GetAnimationFrame();

	int GetWidth();
	int GetHeight();

	void HeadBonk();
	void StompKill();

	bool IsAlive();

	void SetPaused(bool paused);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	static AI_TYPE StringToAIType(std::string aiTypeString);
	static SPAWN_LOCATION_TYPE StringToSpawnLocationType(std::string spawnLocationTypeString);

private:
	void UpdatePosition(double deltaTime);
	void CalculateNewTarget();

	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	static const double HORIZONTAL_ACCELERATION;
	static const double MAX_HORIZONTAL_VEL;

	DOUBLE2 m_SpawingPosition;

	ANIMATION_STATE m_AnimationState;

	double m_TargetOffshoot = 35;
	double m_TargetX;

	// If true, we now can never spawn again, but we need 
	// to continue painting our spawning hole (if we live in the wall, that is)
	// so we can't be removed from the level
	bool m_HasBeenKilledByPlayer = true; 
	
	bool m_HaveSpawnedMole = false; // This is set to true once we've spawned a mole

	static const int FRAMES_TO_WRIGGLE_IN_DIRT_FOR = 90;
	int m_FramesSpentWrigglingInTheDirt = -1;

	bool m_ShouldRemoveActor;

	SPAWN_LOCATION_TYPE m_SpawnLocationType;
	AI_TYPE m_AiType;
};

